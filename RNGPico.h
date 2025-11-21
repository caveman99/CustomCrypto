/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - RP2040/RP2350 hardware RNG support
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
