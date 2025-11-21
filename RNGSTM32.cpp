/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - STM32WL55 hardware RNG support
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "RNGSTM32.h"
#include "Crypto.h"
#include <string.h>

#if defined(STM32WLxx) || defined(STM32WL55xx) || defined(STM32WL54xx) || defined(STM32WLE5xx) || defined(STM32WLE4xx)

// Include STM32 HAL headers
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for STM32 HAL RNG functions
// These are provided by the STM32 HAL library

typedef struct {
    void* Instance;        // RNG peripheral instance
} RNG_HandleTypeDef;

// HAL Status
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// Extern RNG peripheral instance
extern void* RNG;

// HAL Function declarations
void __HAL_RCC_RNG_CLK_ENABLE(void);
void __HAL_RCC_RNG_CLK_DISABLE(void);
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef* hrng);
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef* hrng);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef* hrng, uint32_t* random32bit);

#ifdef __cplusplus
}
#endif

#define RNG_TIMEOUT 5000  // 5 second timeout

/**
 * \brief Constructs a new hardware random number generator.
 */
RNGSTM32::RNGSTM32()
    : _initialized(false)
{
}

/**
 * \brief Destroys this hardware random number generator.
 */
RNGSTM32::~RNGSTM32()
{
    if (_initialized) {
        RNG_HandleTypeDef hrng;
        hrng.Instance = RNG;
        HAL_RNG_DeInit(&hrng);
        __HAL_RCC_RNG_CLK_DISABLE();
        _initialized = false;
    }
}

/**
 * \brief Determines if the hardware RNG is still calibrating.
 *
 * \return Always returns false. The STM32WL55 hardware TRNG is ready
 * once initialized.
 */
bool RNGSTM32::calibrating() const
{
    // Hardware TRNG is ready once initialized
    return !_initialized;
}

/**
 * \brief Stirs fresh entropy from the hardware RNG into the global pool.
 *
 * This function generates 32 bytes of random data from the STM32WL55's
 * hardware TRNG and adds it to the global random number pool with full
 * entropy credit (256 bits).
 */
void RNGSTM32::stir()
{
    if (!_initialized) {
        return;
    }

    // Generate 32 bytes of random data (8 x 32-bit words)
    uint8_t buffer[32];
    uint32_t *words = (uint32_t*)buffer;

    RNG_HandleTypeDef hrng;
    hrng.Instance = RNG;

    // Enable RNG clock
    __HAL_RCC_RNG_CLK_ENABLE();

    bool success = true;
    for (int i = 0; i < 8; i++) {
        if (HAL_RNG_GenerateRandomNumber(&hrng, &words[i]) != HAL_OK) {
            success = false;
            break;
        }
    }

    __HAL_RCC_RNG_CLK_DISABLE();

    if (success) {
        // Credit full entropy - hardware TRNG
        output(buffer, sizeof(buffer), sizeof(buffer) * 8);  // 256 bits
    }

    // Clean up sensitive data
    clean(buffer, sizeof(buffer));
}

/**
 * \brief Called when this noise source is added to the global RNG.
 *
 * Initializes the STM32WL55 hardware TRNG and stirs in initial entropy.
 */
void RNGSTM32::added()
{
    if (_initialized) {
        return;
    }

    // Enable RNG clock
    __HAL_RCC_RNG_CLK_ENABLE();

    // Initialize RNG
    RNG_HandleTypeDef hrng;
    hrng.Instance = RNG;

    if (HAL_RNG_Init(&hrng) == HAL_OK) {
        _initialized = true;

        // Immediately stir in some initial entropy
        stir();
    } else {
        __HAL_RCC_RNG_CLK_DISABLE();
    }
}

#endif // STM32WLxx
