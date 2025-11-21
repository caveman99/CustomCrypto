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

#include "SHA256Pico.h"
#include "Crypto.h"
#include <string.h>

#if defined(RP2350A) || defined(RP2350B)

// Include Pico SDK SHA-256 headers
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for Pico SDK hardware SHA-256 functions
// These are provided by the pico-sdk's hardware_sha256 library

// Configuration functions
void sha256_set_dma_size(uint8_t size);
void sha256_set_bswap(bool enable);

// Control functions
void sha256_start(void);
bool sha256_is_ready(void);
bool sha256_is_sum_valid(void);

// Blocking wait functions
void sha256_wait_ready_blocking(void);
void sha256_wait_valid_blocking(void);

// Data input functions
void sha256_put_word(uint32_t data);
void sha256_put_byte(uint8_t data);

// Result functions
typedef union {
    uint32_t word[8];
    uint8_t byte[32];
} sha256_result_t;

void sha256_get_result(sha256_result_t *result, bool swap_endianness);

#ifdef __cplusplus
}
#endif

/**
 * \brief Constructs a hardware-accelerated SHA-256 hash object.
 */
SHA256Pico::SHA256Pico()
{
    reset();
}

/**
 * \brief Destroys this SHA-256 hash object.
 */
SHA256Pico::~SHA256Pico()
{
    clean(state);
}

/**
 * \brief Gets the size of the hash output.
 * \return Always returns 32 (SHA-256 produces 256-bit/32-byte hashes).
 */
size_t SHA256Pico::hashSize() const
{
    return HASH_SIZE;
}

/**
 * \brief Gets the block size for SHA-256.
 * \return Always returns 64 (SHA-256 processes 512-bit/64-byte blocks).
 */
size_t SHA256Pico::blockSize() const
{
    return BLOCK_SIZE;
}

/**
 * \brief Resets the hash computation to start a new hash.
 */
void SHA256Pico::reset()
{
    state.bufferLen = 0;
    state.totalLen = 0;
    state.hwActive = false;
    memset(state.buffer, 0, sizeof(state.buffer));
}

/**
 * \brief Processes the current buffer using hardware acceleration.
 */
void SHA256Pico::processBuffer()
{
    if (state.bufferLen == 0) {
        return;
    }

    // If this is the first block, initialize hardware
    if (!state.hwActive) {
        // Configure hardware: 4-byte DMA transfers, no byte swapping
        sha256_set_dma_size(4);
        sha256_set_bswap(false);
        sha256_start();
        state.hwActive = true;
    }

    // Feed the 64-byte block to hardware (must be exactly 64 bytes)
    if (state.bufferLen == 64) {
        uint32_t *words = (uint32_t*)state.buffer;

        for (int i = 0; i < 16; i++) {
            sha256_wait_ready_blocking();
            sha256_put_word(words[i]);
        }

        state.bufferLen = 0;
    }
}

/**
 * \brief Updates the hash with additional data.
 *
 * \param data Points to the data to be hashed.
 * \param len Number of bytes of data to be hashed.
 */
void SHA256Pico::update(const void *data, size_t len)
{
    const uint8_t *d = (const uint8_t *)data;
    state.totalLen += len;

    while (len > 0) {
        size_t remaining = BLOCK_SIZE - state.bufferLen;
        size_t copyLen = (len < remaining) ? len : remaining;

        memcpy(state.buffer + state.bufferLen, d, copyLen);
        state.bufferLen += copyLen;
        d += copyLen;
        len -= copyLen;

        if (state.bufferLen == BLOCK_SIZE) {
            processBuffer();
        }
    }
}

/**
 * \brief Finalizes the hash computation and returns the result.
 *
 * \param hash Points to the buffer to receive the hash value.
 * \param len The size of the hash buffer, which should be at least
 * hashSize() bytes but may be truncated.
 */
void SHA256Pico::finalize(void *hash, size_t len)
{
    // Add SHA-256 padding
    uint64_t totalBits = state.totalLen * 8;

    // Append the '1' bit (0x80 byte)
    uint8_t padding[64 + 8];
    padding[0] = 0x80;

    // Calculate padding length
    size_t padLen;
    if (state.bufferLen < 56) {
        padLen = 56 - state.bufferLen;
    } else {
        padLen = 64 + 56 - state.bufferLen;
    }

    // Fill rest with zeros (already part of padding array initialization)
    memset(padding + 1, 0, padLen - 1);

    // Append length in bits as big-endian 64-bit value
    padding[padLen]     = (uint8_t)(totalBits >> 56);
    padding[padLen + 1] = (uint8_t)(totalBits >> 48);
    padding[padLen + 2] = (uint8_t)(totalBits >> 40);
    padding[padLen + 3] = (uint8_t)(totalBits >> 32);
    padding[padLen + 4] = (uint8_t)(totalBits >> 24);
    padding[padLen + 5] = (uint8_t)(totalBits >> 16);
    padding[padLen + 6] = (uint8_t)(totalBits >> 8);
    padding[padLen + 7] = (uint8_t)(totalBits);

    // Update with padding
    update(padding, padLen + 8);

    // Wait for hardware to complete
    if (state.hwActive) {
        sha256_wait_valid_blocking();

        // Get result from hardware
        sha256_result_t result;
        sha256_get_result(&result, false);  // We handle endianness manually

        // Copy to output buffer
        if (len > HASH_SIZE) {
            len = HASH_SIZE;
        }
        memcpy(hash, result.byte, len);

        state.hwActive = false;
    } else {
        // No data was processed, return zero hash
        memset(hash, 0, len);
    }
}

/**
 * \brief Clears the hash state.
 */
void SHA256Pico::clear()
{
    clean(state);
    reset();
}

/**
 * \brief Resets the hash ready to start a HMAC computation.
 *
 * \param key Points to the HMAC key.
 * \param keyLen Size of the HMAC key in bytes.
 */
void SHA256Pico::resetHMAC(const void *key, size_t keyLen)
{
    uint8_t pad[BLOCK_SIZE];
    uint8_t khash[HASH_SIZE];

    // Hash the key if it is longer than the block size.
    reset();
    if (keyLen <= BLOCK_SIZE) {
        memcpy(pad, key, keyLen);
        memset(pad + keyLen, 0, BLOCK_SIZE - keyLen);
    } else {
        update(key, keyLen);
        finalize(khash, HASH_SIZE);
        memcpy(pad, khash, HASH_SIZE);
        memset(pad + HASH_SIZE, 0, BLOCK_SIZE - HASH_SIZE);
        reset();
    }

    // XOR the key with the inner pad and hash it.
    for (uint8_t posn = 0; posn < BLOCK_SIZE; ++posn)
        pad[posn] ^= 0x36;
    update(pad, BLOCK_SIZE);

    // Clean up.
    clean(pad);
    clean(khash);
}

/**
 * \brief Finalizes the HMAC computation.
 *
 * \param key Points to the HMAC key.
 * \param keyLen Size of the HMAC key in bytes.
 * \param hash Points to the buffer to receive the HMAC value.
 * \param hashLen The size of the hash buffer, which should be at least
 * hashSize() bytes but may be truncated.
 */
void SHA256Pico::finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen)
{
    uint8_t pad[BLOCK_SIZE];
    uint8_t khash[HASH_SIZE];
    uint8_t temp[HASH_SIZE];

    // Finalize the inner hash.
    finalize(temp, HASH_SIZE);

    // Hash the key if it is longer than the block size.
    reset();
    if (keyLen <= BLOCK_SIZE) {
        memcpy(pad, key, keyLen);
        memset(pad + keyLen, 0, BLOCK_SIZE - keyLen);
    } else {
        update(key, keyLen);
        finalize(khash, HASH_SIZE);
        memcpy(pad, khash, HASH_SIZE);
        memset(pad + HASH_SIZE, 0, BLOCK_SIZE - HASH_SIZE);
        reset();
    }

    // XOR the key with the outer pad, hash it, and the inner hash.
    for (uint8_t posn = 0; posn < BLOCK_SIZE; ++posn)
        pad[posn] ^= 0x5C;
    update(pad, BLOCK_SIZE);
    update(temp, HASH_SIZE);
    finalize(hash, hashLen);

    // Clean up.
    clean(pad);
    clean(khash);
    clean(temp);
}

#endif // RP2350A || RP2350B
