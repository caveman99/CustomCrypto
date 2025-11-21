/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - RP2350 hardware SHA-256 acceleration
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

#ifndef CRYPTO_SHA256PICO_h
#define CRYPTO_SHA256PICO_h

#include "Hash.h"

// Only available on RP2350 (has hardware SHA-256 accelerator)
// RP2040 should use the standard software SHA256 class
#if defined(RP2350A) || defined(RP2350B)

/**
 * \class SHA256Pico SHA256Pico.h <SHA256Pico.h>
 * \brief Hardware-accelerated SHA-256 implementation for RP2350.
 *
 * This class provides SHA-256 hashing using the RP2350's hardware
 * SHA-256 accelerator, which offers significant performance improvements
 * over software-only implementations.
 *
 * The RP2350 hardware accelerator processes data in 64-byte blocks and
 * produces a 32-byte (256-bit) hash digest.
 *
 * Example usage:
 * \code
 * #include <SHA256Pico.h>
 *
 * SHA256Pico sha256;
 * uint8_t hash[32];
 *
 * sha256.reset();
 * sha256.update("Hello", 5);
 * sha256.update(" World", 6);
 * sha256.finalize(hash, sizeof(hash));
 * \endcode
 *
 * Note: This class is only available on RP2350. On RP2040, use the
 * standard SHA256 software implementation.
 *
 * References:
 * - https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#pico_sha256
 * - https://github.com/raspberrypi/pico-sdk/tree/master/src/rp2_common/hardware_sha256
 *
 * \sa SHA256
 */
class SHA256Pico : public Hash
{
public:
    SHA256Pico();
    virtual ~SHA256Pico();

    size_t hashSize() const;
    size_t blockSize() const;

    void reset();
    void update(const void *data, size_t len);
    void finalize(void *hash, size_t len);

    void clear();

    void resetHMAC(const void *key, size_t keyLen);
    void finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen);

    static const size_t HASH_SIZE  = 32;
    static const size_t BLOCK_SIZE = 64;

private:
    struct {
        uint8_t buffer[64];     // Buffer for incomplete blocks
        uint32_t bufferLen;     // Current buffer length
        uint64_t totalLen;      // Total bytes processed
        bool hwActive;          // Hardware calculation in progress
    } state;

    void processBuffer();
};

#endif // RP2350A || RP2350B

#endif // CRYPTO_SHA256PICO_h
