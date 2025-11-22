/*
 * STM32WL55 Hardware RNG Support
 * Copyright (C) 2025 caveman99
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
