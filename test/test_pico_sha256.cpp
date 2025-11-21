/*
 * Test cases for RP2350 hardware-accelerated SHA-256
 */

#include <unity.h>

#if defined(RP2350A) || defined(RP2350B)
#include <SHA256Pico.h>
#include <string.h>

// NIST test vectors for SHA-256
// Test case: empty string
static const char* test1_input = "";
static const uint8_t test1_expected[] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
    0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
    0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
};

// Test case: "abc"
static const char* test2_input = "abc";
static const uint8_t test2_expected[] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

// Test case: longer message
static const char* test3_input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const uint8_t test3_expected[] = {
    0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
    0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
    0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
    0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

void test_sha256_empty_string(void) {
    SHA256Pico sha256;
    uint8_t hash[32];

    sha256.reset();
    sha256.update(test1_input, strlen(test1_input));
    sha256.finalize(hash, sizeof(hash));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test1_expected, hash, 32);
}

void test_sha256_abc(void) {
    SHA256Pico sha256;
    uint8_t hash[32];

    sha256.reset();
    sha256.update(test2_input, strlen(test2_input));
    sha256.finalize(hash, sizeof(hash));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, hash, 32);
}

void test_sha256_long_message(void) {
    SHA256Pico sha256;
    uint8_t hash[32];

    sha256.reset();
    sha256.update(test3_input, strlen(test3_input));
    sha256.finalize(hash, sizeof(hash));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test3_expected, hash, 32);
}

void test_sha256_incremental_update(void) {
    SHA256Pico sha256;
    uint8_t hash[32];

    // Hash "abc" in three separate updates
    sha256.reset();
    sha256.update("a", 1);
    sha256.update("b", 1);
    sha256.update("c", 1);
    sha256.finalize(hash, sizeof(hash));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(test2_expected, hash, 32);
}

void test_sha256_multiple_blocks(void) {
    SHA256Pico sha256;
    uint8_t hash[32];

    // Create a message that spans multiple 64-byte blocks
    char message[200];
    memset(message, 'a', sizeof(message));
    message[sizeof(message) - 1] = '\0';

    sha256.reset();
    sha256.update(message, strlen(message));
    sha256.finalize(hash, sizeof(hash));

    // Just verify it produces consistent results
    uint8_t hash2[32];
    sha256.reset();
    sha256.update(message, strlen(message));
    sha256.finalize(hash2, sizeof(hash2));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(hash, hash2, 32);
}

void test_sha256_hmac(void) {
    SHA256Pico sha256;
    uint8_t key[] = "secret";
    uint8_t message[] = "The quick brown fox jumps over the lazy dog";
    uint8_t hmac[32];

    sha256.resetHMAC(key, sizeof(key) - 1);
    sha256.update(message, sizeof(message) - 1);
    sha256.finalizeHMAC(key, sizeof(key) - 1, hmac, sizeof(hmac));

    // Just verify it produces consistent results
    uint8_t hmac2[32];
    sha256.resetHMAC(key, sizeof(key) - 1);
    sha256.update(message, sizeof(message) - 1);
    sha256.finalizeHMAC(key, sizeof(key) - 1, hmac2, sizeof(hmac2));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac, hmac2, 32);
}

void test_sha256_constants(void) {
    SHA256Pico sha256;
    TEST_ASSERT_EQUAL(32, sha256.hashSize());
    TEST_ASSERT_EQUAL(64, sha256.blockSize());
}

void setUp(void) {
    // Nothing to set up
}

void tearDown(void) {
    // Nothing to tear down
}

#ifdef ARDUINO

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_sha256_empty_string);
    RUN_TEST(test_sha256_abc);
    RUN_TEST(test_sha256_long_message);
    RUN_TEST(test_sha256_incremental_update);
    RUN_TEST(test_sha256_multiple_blocks);
    RUN_TEST(test_sha256_hmac);
    RUN_TEST(test_sha256_constants);

    UNITY_END();
}

void loop() {
    // Nothing to do
}

#else

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_sha256_empty_string);
    RUN_TEST(test_sha256_abc);
    RUN_TEST(test_sha256_long_message);
    RUN_TEST(test_sha256_incremental_update);
    RUN_TEST(test_sha256_multiple_blocks);
    RUN_TEST(test_sha256_hmac);
    RUN_TEST(test_sha256_constants);

    return UNITY_END();
}

#endif

#else

// Dummy tests for non-RP2350 platforms
void test_platform_not_supported(void) {
    TEST_IGNORE_MESSAGE("RP2350 SHA-256 tests only run on RP2350 hardware");
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

#endif // RP2350A || RP2350B
