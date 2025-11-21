/*
 * Test cases for ESP32 hardware-accelerated AES
 * Supports ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-C2
 */

#include <unity.h>

#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || \
    defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3) || \
    defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C2)

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

void test_aes256_roundtrip(void) {
    AES256 aes;
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    const char* message = "ESP32 AES Test!!";

    TEST_ASSERT_TRUE(aes.setKey(aes256_key, sizeof(aes256_key)));
    aes.encryptBlock(encrypted, (const uint8_t*)message);
    aes.decryptBlock(decrypted, encrypted);
    TEST_ASSERT_EQUAL_STRING(message, (const char*)decrypted);
}

void test_aes128_ctr_mode(void) {
    CTR<AES128> ctr;
    uint8_t key[16], iv[16], plaintext[32], ciphertext[32], decrypted[32];

    // Initialize test data
    memset(key, 0x01, sizeof(key));
    memset(iv, 0x02, sizeof(iv));
    memset(plaintext, 0x03, sizeof(plaintext));

    // Encrypt
    ctr.setKey(key, sizeof(key));
    ctr.setIV(iv, sizeof(iv));
    ctr.encrypt(ciphertext, plaintext, sizeof(plaintext));

    // Decrypt
    ctr.setKey(key, sizeof(key));
    ctr.setIV(iv, sizeof(iv));
    ctr.decrypt(decrypted, ciphertext, sizeof(ciphertext));

    // Verify
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, decrypted, sizeof(plaintext));
}

void test_aes_key_sizes(void) {
    AES128 aes128;
    AES192 aes192;
    AES256 aes256;

    TEST_ASSERT_EQUAL_UINT(16, aes128.keySize());
    TEST_ASSERT_EQUAL_UINT(24, aes192.keySize());
    TEST_ASSERT_EQUAL_UINT(32, aes256.keySize());

    TEST_ASSERT_EQUAL_UINT(16, aes128.blockSize());
    TEST_ASSERT_EQUAL_UINT(16, aes192.blockSize());
    TEST_ASSERT_EQUAL_UINT(16, aes256.blockSize());
}

void test_esp32_variant(void) {
    // Print which ESP32 variant is being tested
#if defined(CONFIG_IDF_TARGET_ESP32)
    TEST_MESSAGE("Running on ESP32 (classic) with hardware AES");
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    TEST_MESSAGE("Running on ESP32-S2 with hardware AES");
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    TEST_MESSAGE("Running on ESP32-S3 with hardware AES");
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    TEST_MESSAGE("Running on ESP32-C3 with hardware AES");
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    TEST_MESSAGE("Running on ESP32-C6 with hardware AES");
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
    TEST_MESSAGE("Running on ESP32-C2 with hardware AES");
#else
    TEST_MESSAGE("Running on ESP32 with hardware AES");
#endif
    TEST_ASSERT_TRUE(true);  // Marker test
}

void setUp(void) {
    // Nothing to set up
}

void tearDown(void) {
    // Nothing to clean up
}

#ifdef ARDUINO

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_esp32_variant);
    RUN_TEST(test_aes128_encrypt);
    RUN_TEST(test_aes128_decrypt);
    RUN_TEST(test_aes128_roundtrip);
    RUN_TEST(test_aes192_encrypt);
    RUN_TEST(test_aes192_decrypt);
    RUN_TEST(test_aes256_encrypt);
    RUN_TEST(test_aes256_decrypt);
    RUN_TEST(test_aes256_roundtrip);
    RUN_TEST(test_aes128_ctr_mode);
    RUN_TEST(test_aes_key_sizes);

    UNITY_END();
}

void loop() {
    // Nothing to do
}

#else

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_esp32_variant);
    RUN_TEST(test_aes128_encrypt);
    RUN_TEST(test_aes128_decrypt);
    RUN_TEST(test_aes128_roundtrip);
    RUN_TEST(test_aes192_encrypt);
    RUN_TEST(test_aes192_decrypt);
    RUN_TEST(test_aes256_encrypt);
    RUN_TEST(test_aes256_decrypt);
    RUN_TEST(test_aes256_roundtrip);
    RUN_TEST(test_aes128_ctr_mode);
    RUN_TEST(test_aes_key_sizes);

    return UNITY_END();
}

#endif

#else

// Dummy tests for non-ESP32 platforms
void test_platform_not_supported(void) {
    TEST_IGNORE_MESSAGE("ESP32 AES tests only run on ESP32 platforms");
}

#ifdef ARDUINO
void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_platform_not_supported);
    UNITY_END();
}
void loop() {}
#else
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_platform_not_supported);
    return UNITY_END();
}
#endif

#endif // ESP32 variants
