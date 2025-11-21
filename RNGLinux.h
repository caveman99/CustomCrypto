/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - Linux hardware RNG support
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

#ifndef CRYPTO_RNG_LINUX_h
#define CRYPTO_RNG_LINUX_h

#include "NoiseSource.h"

#if defined(__linux__) || defined(__unix__) || defined(PORTDUINO)

/**
 * \class RNGLinux RNGLinux.h <RNGLinux.h>
 * \brief Hardware random number generator for Linux systems.
 *
 * This class implements a NoiseSource that uses the Linux kernel's random
 * number generator interface. The kernel automatically mixes hardware RNG
 * sources (such as the BCM2835/BCM2711 hardware RNG on Raspberry Pi) into
 * its entropy pool.
 *
 * The implementation uses:
 * - getrandom() syscall (preferred, available since Linux 3.17)
 * - Falls back to reading /dev/urandom on older kernels
 *
 * On Raspberry Pi systems, this automatically benefits from:
 * - BCM2835 (Pi 1/Zero): Hardware RNG via bcm2835-rng driver
 * - BCM2711 (Pi 4): RNG200 hardware via iproc-rng200 driver
 * - BCM2837 (Pi 3): Hardware RNG via bcm2835-rng driver
 *
 * The Linux kernel's RNG provides cryptographically secure random numbers
 * that are suitable for key generation and other security-critical operations.
 *
 * Example usage:
 * \code
 * #include <Crypto.h>
 * #include <RNG.h>
 * #include <RNGLinux.h>
 *
 * RNGLinux hwRNG;
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
 * - https://man7.org/linux/man-pages/man2/getrandom.2.html
 * - https://docs.kernel.org/admin-guide/hw_random.html
 * - https://github.com/torvalds/linux/blob/master/drivers/char/hw_random/bcm2835-rng.c
 */
class RNGLinux : public NoiseSource
{
public:
    RNGLinux();
    ~RNGLinux();

    bool calibrating() const;
    void stir();
    void added();

private:
    bool _initialized;
    int _urandom_fd;  // File descriptor for /dev/urandom fallback
};

#endif // __linux__ || __unix__ || PORTDUINO

#endif // CRYPTO_RNG_LINUX_h
