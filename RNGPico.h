/*
 * RP2040/RP2350 Hardware RNG Support
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

#ifndef CRYPTO_RNG_PICO_h
#define CRYPTO_RNG_PICO_h

#include "NoiseSource.h"

#if defined(PICO_BOARD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO)

/**
 * \class RNGPico RNGPico.h <RNGPico.h>
 * \brief Hardware random number generator for RP2040/RP2350 devices.
 *
 * This class implements a NoiseSource that uses the Pico SDK's random number
 * generator. The implementation automatically adapts based on the chip:
 *
 * - RP2040: Uses Ring Oscillator (ROSC) as entropy source (lower quality)
 * - RP2350: Uses hardware TRNG (True Random Number Generator) (high quality)
 *
 * The Pico SDK's get_rand_32() function provides random numbers seeded with
 * hardware entropy and enhanced through a software PRNG (xoroshiro128**).
 *
 * Example usage:
 * \code
 * #include <Crypto.h>
 * #include <RNG.h>
 * #include <RNGPico.h>
 *
 * RNGPico hwRNG;
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
 * - https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#pico_rand
 * - https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_rand/
 */
class RNGPico : public NoiseSource
{
public:
    RNGPico();
    ~RNGPico();

    bool calibrating() const;
    void stir();
    void added();

private:
    bool _initialized;
};

#endif // PICO_BOARD || ARDUINO_ARCH_RP2040

#endif // CRYPTO_RNG_PICO_h
