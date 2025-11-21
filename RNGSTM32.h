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

#ifndef CRYPTO_RNG_STM32_h
#define CRYPTO_RNG_STM32_h

#include "NoiseSource.h"

// STM32WL55 and similar chips with hardware TRNG
#if defined(STM32WLxx) || defined(STM32WL55xx) || defined(STM32WL54xx) || defined(STM32WLE5xx) || defined(STM32WLE4xx)

/**
 * \class RNGSTM32 RNGSTM32.h <RNGSTM32.h>
 * \brief Hardware random number generator for STM32WL55.
 *
 * This class implements a NoiseSource that uses the STM32WL55's hardware
 * True Random Number Generator (TRNG). The TRNG provides high-quality
 * cryptographically secure random numbers directly from hardware.
 *
 * The STM32WL55 TRNG uses analog circuitry and thermal noise to generate
 * genuine random numbers, providing 32-bit random values with full entropy.
 *
 * Example usage:
 * \code
 * #include <Crypto.h>
 * #include <RNG.h>
 * #include <RNGSTM32.h>
 *
 * RNGSTM32 hwRNG;
 *
 * void setup() {
 *     CryptRNG.begin("MyApp 1.0");
 *     CryptRNG.addNoiseSource(hwRNG);
 * }
 *
 * void loop() {
 *     uint8_t random_data[32];
 *     CryptRNG.rand(random_data, sizeof(random_data));
 *     // Use random_data...
 * }
 * \endcode
 *
 * References:
 * - STM32WL5x Reference Manual (RM0453), Section on RNG
 * - STM32WL HAL Driver Documentation (UM2642)
 * - AN4230: STM32 microcontroller random number generation validation
 *
 * \sa RNGClass
 */
class RNGSTM32 : public NoiseSource
{
public:
    RNGSTM32();
    ~RNGSTM32();

    bool calibrating() const;
    void stir();
    void added();

private:
    bool _initialized;
};

#endif // STM32WLxx

#endif // CRYPTO_RNG_STM32_h
