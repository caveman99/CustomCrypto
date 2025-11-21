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

#ifndef CRYPTO_AESSTM32_h
#define CRYPTO_AESSTM32_h

#include "BlockCipher.h"

// Only available on STM32WL55 and similar chips with AES hardware accelerator
// Note: STM32WL55 has AES peripheral (not full CRYP), supports AES-128 and AES-256
#if defined(STM32WLxx) || defined(STM32WL55xx) || defined(STM32WL54xx) || defined(STM32WLE5xx) || defined(STM32WLE4xx)

/**
 * \class AESSTM32 AESSTM32.h <AESSTM32.h>
 * \brief Hardware-accelerated AES implementation for STM32WL55.
 *
 * This class provides AES encryption using the STM32WL55's hardware AES
 * accelerator, which offers significant performance improvements over
 * software-only implementations.
 *
 * The STM32WL55 AES peripheral supports:
 * - AES-128 and AES-256 (192-bit keys use software fallback)
 * - ECB mode (used for block cipher operations)
 * - Hardware acceleration via STM32 HAL_CRYP functions
 *
 * Example usage:
 * \code
 * #include <AESSTM32.h>
 *
 * AESSTM32_128 aes;
 * uint8_t key[16] = { ... };
 * uint8_t plaintext[16] = { ... };
 * uint8_t ciphertext[16];
 *
 * aes.setKey(key, 16);
 * aes.encryptBlock(ciphertext, plaintext);
 * \endcode
 *
 * \note AES-192 is not hardware accelerated and falls back to software.
 * \note Requires STM32 HAL library with CRYP/AES peripheral support.
 *
 * References:
 * - STM32WL5x Reference Manual (RM0453)
 * - STM32WL HAL Driver Documentation (UM2642)
 */

// Forward declare software AES for fallback
class AESCommonSoftware;

// AES-128: Hardware-accelerated
class AESSTM32_128 : public BlockCipher
{
public:
    AESSTM32_128();
    virtual ~AESSTM32_128();

    size_t blockSize() const;
    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t _key[16];
    bool _initialized;
};

// AES-256: Hardware-accelerated
class AESSTM32_256 : public BlockCipher
{
public:
    AESSTM32_256();
    virtual ~AESSTM32_256();

    size_t blockSize() const;
    size_t keySize() const;

    bool setKey(const uint8_t *key, size_t len);

    void encryptBlock(uint8_t *output, const uint8_t *input);
    void decryptBlock(uint8_t *output, const uint8_t *input);

    void clear();

private:
    uint8_t _key[32];
    bool _initialized;
};

// Type aliases for compatibility
typedef AESSTM32_128 AES128STM32;
typedef AESSTM32_256 AES256STM32;

#endif // STM32WLxx

#endif // CRYPTO_AESSTM32_h
