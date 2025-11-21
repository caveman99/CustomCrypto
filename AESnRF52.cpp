/*
 * Copyright (C) 2015,2018 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - nRF52840 hardware acceleration support
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

#include "AES.h"
#include "Crypto.h"
#include <string.h>

#if defined(CRYPTO_AES_NRF52)

// Include software AES common implementation for 192/256-bit keys
#define CRYPTO_AES_DEFAULT 1
#include "AESCommon.cpp"
#include "AES192.cpp"
#include "AES256.cpp"
#undef CRYPTO_AES_DEFAULT

// Hardware AES-128 implementation using CC310
#ifdef __cplusplus
extern "C" {
#endif

// CC310 function declarations
int SaSi_LibInit(void);
void SaSi_LibFini(void);

typedef void* SaSiAesUserContext_t;
typedef enum { SASI_AES_ENCRYPT = 0, SASI_AES_DECRYPT = 1 } SaSiAesEncryptMode_t;
typedef enum { SASI_AES_MODE_ECB = 0 } SaSiAesOperationMode_t;
typedef enum { SASI_AES_PADDING_NONE = 0 } SaSiAesPaddingType_t;
typedef enum { SASI_AES_USER_KEY = 0 } SaSiAesKeyType_t;

int SaSi_AesInit(SaSiAesUserContext_t* ctx, SaSiAesEncryptMode_t encMode,
                 SaSiAesOperationMode_t opMode, SaSiAesPaddingType_t padType);
int SaSi_AesSetKey(SaSiAesUserContext_t* ctx, SaSiAesKeyType_t keyType,
                   const uint8_t* key, size_t keySizeBits);
int SaSi_AesBlock(SaSiAesUserContext_t* ctx, const uint8_t* in,
                  size_t inSize, uint8_t* out);
int SaSi_AesFinish(SaSiAesUserContext_t* ctx, size_t* outSize);

#ifdef __cplusplus
}
#endif

#define NRF52_AES_CONTEXT_SIZE 256

// AES-128: Hardware-accelerated implementation

AES128::AES128()
    : _initialized(false)
{
    memset(_key, 0, sizeof(_key));
}

AES128::~AES128()
{
    clean(_key, sizeof(_key));
}

size_t AES128::blockSize() const
{
    return 16;
}

size_t AES128::keySize() const
{
    return 16;
}

bool AES128::setKey(const uint8_t *key, size_t len)
{
    if (len != 16)
        return false;

    memcpy(_key, key, 16);
    _initialized = true;
    return true;
}

void AES128::encryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    uint8_t contextBuffer[NRF52_AES_CONTEXT_SIZE];
    SaSiAesUserContext_t* context = (SaSiAesUserContext_t*)contextBuffer;
    memset(contextBuffer, 0, sizeof(contextBuffer));

    SaSi_LibInit();

    int result = SaSi_AesInit(context, SASI_AES_ENCRYPT,
                              SASI_AES_MODE_ECB, SASI_AES_PADDING_NONE);

    if (result == 0) {
        result = SaSi_AesSetKey(context, SASI_AES_USER_KEY, _key, 128);
        if (result == 0) {
            result = SaSi_AesBlock(context, input, 16, output);
            size_t outSize = 0;
            SaSi_AesFinish(context, &outSize);
        }
    }

    SaSi_LibFini();
    clean(contextBuffer, sizeof(contextBuffer));

    if (result != 0) {
        memcpy(output, input, 16);
    }
}

void AES128::decryptBlock(uint8_t *output, const uint8_t *input)
{
    if (!_initialized) {
        memset(output, 0, 16);
        return;
    }

    uint8_t contextBuffer[NRF52_AES_CONTEXT_SIZE];
    SaSiAesUserContext_t* context = (SaSiAesUserContext_t*)contextBuffer;
    memset(contextBuffer, 0, sizeof(contextBuffer));

    SaSi_LibInit();

    int result = SaSi_AesInit(context, SASI_AES_DECRYPT,
                              SASI_AES_MODE_ECB, SASI_AES_PADDING_NONE);

    if (result == 0) {
        result = SaSi_AesSetKey(context, SASI_AES_USER_KEY, _key, 128);
        if (result == 0) {
            result = SaSi_AesBlock(context, input, 16, output);
            size_t outSize = 0;
            SaSi_AesFinish(context, &outSize);
        }
    }

    SaSi_LibFini();
    clean(contextBuffer, sizeof(contextBuffer));

    if (result != 0) {
        memcpy(output, input, 16);
    }
}

void AES128::clear()
{
    clean(_key, sizeof(_key));
    _initialized = false;
}

// Tiny/Small 128 - just use hardware AES128

AESTiny128::AESTiny128() {
    memset(schedule, 0, sizeof(schedule));
}

AESTiny128::~AESTiny128() {
    clean(schedule);
}

size_t AESTiny128::blockSize() const {
    return 16;
}

size_t AESTiny128::keySize() const {
    return 16;
}

bool AESTiny128::setKey(const uint8_t *key, size_t len) {
    if (len != 16)
        return false;
    memcpy(schedule, key, 16);
    return true;
}

void AESTiny128::encryptBlock(uint8_t *output, const uint8_t *input) {
    AES128 aes;
    aes.setKey(schedule, 16);
    aes.encryptBlock(output, input);
    aes.clear();
}

void AESTiny128::decryptBlock(uint8_t *output, const uint8_t *input) {
    AES128 aes;
    aes.setKey(schedule, 16);
    aes.decryptBlock(output, input);
    aes.clear();
}

void AESTiny128::clear() {
    clean(schedule);
}

AESSmall128::AESSmall128() {
    memset(reverse, 0, sizeof(reverse));
}

AESSmall128::~AESSmall128() {
    clean(reverse);
}

bool AESSmall128::setKey(const uint8_t *key, size_t len) {
    if (!AESTiny128::setKey(key, len))
        return false;
    memcpy(reverse, key, 16);
    return true;
}

void AESSmall128::decryptBlock(uint8_t *output, const uint8_t *input) {
    AESTiny128::decryptBlock(output, input);
}

void AESSmall128::clear() {
    clean(reverse);
    AESTiny128::clear();
}

// Re-export AESCommonSoftware as it was renamed
AESCommonSoftware::AESCommonSoftware() : AESCommon() {}
AESCommonSoftware::~AESCommonSoftware() {}
size_t AESCommonSoftware::blockSize() const { return AESCommon::blockSize(); }
void AESCommonSoftware::encryptBlock(uint8_t *output, const uint8_t *input) {
    AESCommon::encryptBlock(output, input);
}
void AESCommonSoftware::decryptBlock(uint8_t *output, const uint8_t *input) {
    AESCommon::decryptBlock(output, input);
}
void AESCommonSoftware::clear() { AESCommon::clear(); }

#endif // CRYPTO_AES_NRF52
