/*
 * Copyright (C) 2015,2018 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - STM32WL55 hardware acceleration support
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

#include "AESSTM32.h"
#include "Crypto.h"
#include <string.h>

#if defined(STM32WLxx) || defined(STM32WL55xx) || defined(STM32WL54xx) || defined(STM32WLE5xx) || defined(STM32WLE4xx)

// Include STM32 HAL headers
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for STM32 HAL CRYP functions
// These are provided by the STM32 HAL library
typedef struct {
    void* Instance;        // AES peripheral instance
    struct {
        uint32_t DataType;
        uint32_t KeySize;
        uint32_t* pKey;
        uint32_t Algorithm;
        uint32_t DataWidthUnit;
    } Init;
} CRYP_HandleTypeDef;

// CRYP Data Types
#define CRYP_DATATYPE_32B     0x00000000U
#define CRYP_DATATYPE_16B     0x00000001U
#define CRYP_DATATYPE_8B      0x00000002U
#define CRYP_DATATYPE_1B      0x00000003U

// CRYP Key Size
#define CRYP_KEYSIZE_128B     0x00000000U
#define CRYP_KEYSIZE_256B     0x00000001U

// CRYP Algorithm
#define CRYP_AES_ECB          0x00000000U

// CRYP Data Width Unit
#define CRYP_DATAWIDTHUNIT_WORD 0x00000000U
#define CRYP_DATAWIDTHUNIT_BYTE 0x00000001U

// HAL Status
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// Extern AES peripheral instance
extern void* AES;

// HAL Function declarations
void __HAL_RCC_AES_CLK_ENABLE(void);
void __HAL_RCC_AES_CLK_DISABLE(void);
HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef* hcryp);
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef* hcryp);
HAL_StatusTypeDef HAL_CRYP_Encrypt(CRYP_HandleTypeDef* hcryp, uint32_t* pInput, uint16_t Size, uint32_t* pOutput, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_Decrypt(CRYP_HandleTypeDef* hcryp, uint32_t* pInput, uint16_t Size, uint32_t* pOutput, uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#define AES_TIMEOUT 5000  // 5 second timeout

// =============================================================================
// AES-128 Implementation
// =============================================================================

AESSTM32_128::AESSTM32_128()
    : _initialized(false)
{
    memset(_key, 0, sizeof(_key));
}

AESSTM32_128::~AESSTM32_128()
{
    clean(_key, sizeof(_key));
}

size_t AESSTM32_128::blockSize() const
{
    return 16;
}

size_t AESSTM32_128::keySize() const
{
    return 16;
}

bool AESSTM32_128::setKey(const uint8_t *key, size_t len)
{
    if (len != 16)
        return false;

    memcpy(_key, key, 16);
    _initialized = true;
    return true;
}

void AESSTM32_128::encryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    // Enable AES clock
    __HAL_RCC_AES_CLK_ENABLE();

    // Setup CRYP handle
    CRYP_HandleTypeDef hcryp;
    memset(&hcryp, 0, sizeof(hcryp));

    hcryp.Instance = AES;
    hcryp.Init.DataType = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
    hcryp.Init.pKey = (uint32_t*)_key;
    hcryp.Init.Algorithm = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;

    // Initialize
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        memcpy(output, input, 16);
        __HAL_RCC_AES_CLK_DISABLE();
        return;
    }

    // Encrypt
    if (HAL_CRYP_Encrypt(&hcryp, (uint32_t*)input, 16, (uint32_t*)output, AES_TIMEOUT) != HAL_OK) {
        memcpy(output, input, 16);
    }

    // Cleanup
    HAL_CRYP_DeInit(&hcryp);
    __HAL_RCC_AES_CLK_DISABLE();
}

void AESSTM32_128::decryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    // Enable AES clock
    __HAL_RCC_AES_CLK_ENABLE();

    // Setup CRYP handle
    CRYP_HandleTypeDef hcryp;
    memset(&hcryp, 0, sizeof(hcryp));

    hcryp.Instance = AES;
    hcryp.Init.DataType = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
    hcryp.Init.pKey = (uint32_t*)_key;
    hcryp.Init.Algorithm = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;

    // Initialize
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        memcpy(output, input, 16);
        __HAL_RCC_AES_CLK_DISABLE();
        return;
    }

    // Decrypt
    if (HAL_CRYP_Decrypt(&hcryp, (uint32_t*)input, 16, (uint32_t*)output, AES_TIMEOUT) != HAL_OK) {
        memcpy(output, input, 16);
    }

    // Cleanup
    HAL_CRYP_DeInit(&hcryp);
    __HAL_RCC_AES_CLK_DISABLE();
}

void AESSTM32_128::clear()
{
    clean(_key, sizeof(_key));
    _initialized = false;
}

// =============================================================================
// AES-256 Implementation
// =============================================================================

AESSTM32_256::AESSTM32_256()
    : _initialized(false)
{
    memset(_key, 0, sizeof(_key));
}

AESSTM32_256::~AESSTM32_256()
{
    clean(_key, sizeof(_key));
}

size_t AESSTM32_256::blockSize() const
{
    return 16;
}

size_t AESSTM32_256::keySize() const
{
    return 32;
}

bool AESSTM32_256::setKey(const uint8_t *key, size_t len)
{
    if (len != 32)
        return false;

    memcpy(_key, key, 32);
    _initialized = true;
    return true;
}

void AESSTM32_256::encryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    // Enable AES clock
    __HAL_RCC_AES_CLK_ENABLE();

    // Setup CRYP handle
    CRYP_HandleTypeDef hcryp;
    memset(&hcryp, 0, sizeof(hcryp));

    hcryp.Instance = AES;
    hcryp.Init.DataType = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
    hcryp.Init.pKey = (uint32_t*)_key;
    hcryp.Init.Algorithm = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;

    // Initialize
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        memcpy(output, input, 16);
        __HAL_RCC_AES_CLK_DISABLE();
        return;
    }

    // Encrypt
    if (HAL_CRYP_Encrypt(&hcryp, (uint32_t*)input, 16, (uint32_t*)output, AES_TIMEOUT) != HAL_OK) {
        memcpy(output, input, 16);
    }

    // Cleanup
    HAL_CRYP_DeInit(&hcryp);
    __HAL_RCC_AES_CLK_DISABLE();
}

void AESSTM32_256::decryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    // Enable AES clock
    __HAL_RCC_AES_CLK_ENABLE();

    // Setup CRYP handle
    CRYP_HandleTypeDef hcryp;
    memset(&hcryp, 0, sizeof(hcryp));

    hcryp.Instance = AES;
    hcryp.Init.DataType = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
    hcryp.Init.pKey = (uint32_t*)_key;
    hcryp.Init.Algorithm = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;

    // Initialize
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        memcpy(output, input, 16);
        __HAL_RCC_AES_CLK_DISABLE();
        return;
    }

    // Decrypt
    if (HAL_CRYP_Decrypt(&hcryp, (uint32_t*)input, 16, (uint32_t*)output, AES_TIMEOUT) != HAL_OK) {
        memcpy(output, input, 16);
    }

    // Cleanup
    HAL_CRYP_DeInit(&hcryp);
    __HAL_RCC_AES_CLK_DISABLE();
}

void AESSTM32_256::clear()
{
    clean(_key, sizeof(_key));
    _initialized = false;
}

#endif // STM32WLxx
