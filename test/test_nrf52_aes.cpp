/*
 * Test cases for nRF52840 hardware-accelerated AES
 */

#include <unity.h>
#include <AES.h>
#include <CTR.h>
#include <string.h>

// NIST test vectors for AES-128
static const uint8_t aes128_key[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static const uint8_t aes128_plaintext[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

static const uint8_t aes128_ciphertext[] = {
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
    0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

// NIST test vectors for AES-192
static const uint8_t aes192_key[] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};

static const uint8_t aes192_plaintext[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

static const uint8_t aes192_ciphertext[] = {
    0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f,
    0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc
};

// NIST test vectors for AES-256
static const uint8_t aes256_key[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

static const uint8_t aes256_plaintext[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

static const uint8_t aes256_ciphertext[] = {
    0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
    0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8
};

void test_aes128_encrypt(void) {
    AES128 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes128_key, sizeof(aes128_key)));
    aes.encryptBlock(output, aes128_plaintext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes128_ciphertext, output, 16);
}

void test_aes128_decrypt(void) {
    AES128 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes128_key, sizeof(aes128_key)));
    aes.decryptBlock(output, aes128_ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes128_plaintext, output, 16);
}

void test_aes128_roundtrip(void) {
    AES128 aes;
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    const char* message = "Hello World!1234";

    TEST_ASSERT_TRUE(aes.setKey(aes128_key, sizeof(aes128_key)));
    aes.encryptBlock(encrypted, (const uint8_t*)message);
    aes.decryptBlock(decrypted, encrypted);
    TEST_ASSERT_EQUAL_STRING(message, (const char*)decrypted);
}

void test_aes192_encrypt(void) {
    AES192 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes192_key, sizeof(aes192_key)));
    aes.encryptBlock(output, aes192_plaintext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes192_ciphertext, output, 16);
}

void test_aes192_decrypt(void) {
    AES192 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes192_key, sizeof(aes192_key)));
    aes.decryptBlock(output, aes192_ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes192_plaintext, output, 16);
}

void test_aes256_encrypt(void) {
    AES256 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes256_key, sizeof(aes256_key)));
    aes.encryptBlock(output, aes256_plaintext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes256_ciphertext, output, 16);
}

void test_aes256_decrypt(void) {
    AES256 aes;
    uint8_t output[16];

    TEST_ASSERT_TRUE(aes.setKey(aes256_key, sizeof(aes256_key)));
    aes.decryptBlock(output, aes256_ciphertext);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(aes256_plaintext, output, 16);
}

void test_aes128_ctr_mode(void) {
    CTR<AES128> ctr;
    uint8_t key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                       0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    uint8_t iv[16] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                      0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};
    const char* message = "The quick brown fox jumps over the lazy dog";
    size_t len = strlen(message);
    uint8_t encrypted[64];
    uint8_t decrypted[64];

    ctr.setKey(key, 16);
    ctr.setIV(iv, 16);
    ctr.encrypt(encrypted, (const uint8_t*)message, len);

    // Reset IV for decryption
    ctr.setIV(iv, 16);
    ctr.decrypt(decrypted, encrypted, len);
    decrypted[len] = '\0';

    TEST_ASSERT_EQUAL_STRING(message, (const char*)decrypted);
}

void test_aes_key_size_validation(void) {
    AES128 aes128;
    AES192 aes192;
    AES256 aes256;
    uint8_t wrong_key[8] = {0};

    TEST_ASSERT_FALSE(aes128.setKey(wrong_key, 8));
    TEST_ASSERT_FALSE(aes192.setKey(wrong_key, 8));
    TEST_ASSERT_FALSE(aes256.setKey(wrong_key, 8));
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}

#ifdef ARDUINO

void setup() {
    delay(2000);  // Wait for serial
    UNITY_BEGIN();

    RUN_TEST(test_aes128_encrypt);
    RUN_TEST(test_aes128_decrypt);
    RUN_TEST(test_aes128_roundtrip);
    RUN_TEST(test_aes192_encrypt);
    RUN_TEST(test_aes192_decrypt);
    RUN_TEST(test_aes256_encrypt);
    RUN_TEST(test_aes256_decrypt);
    RUN_TEST(test_aes128_ctr_mode);
    RUN_TEST(test_aes_key_size_validation);

    UNITY_END();
}

void loop() {
    // Nothing to do
}

#else

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_aes128_encrypt);
    RUN_TEST(test_aes128_decrypt);
    RUN_TEST(test_aes128_roundtrip);
    RUN_TEST(test_aes192_encrypt);
    RUN_TEST(test_aes192_decrypt);
    RUN_TEST(test_aes256_encrypt);
    RUN_TEST(test_aes256_decrypt);
    RUN_TEST(test_aes128_ctr_mode);
    RUN_TEST(test_aes_key_size_validation);

    return UNITY_END();
}

#endif
