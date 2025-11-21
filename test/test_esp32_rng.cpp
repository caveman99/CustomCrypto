/*
 * Test cases for ESP32 hardware RNG
 * Supports ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-C2
 */

#include <unity.h>

#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || \
    defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3) || \
    defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C2)

#include <RNG.h>
#include <RNGEsp32.h>
#include <string.h>

RNGEsp32 hwRNG;
bool rng_initialized = false;

void test_rng_initialization(void) {
    if (!rng_initialized) {
        CryptRNG.begin("ESP32 Test Suite");
        CryptRNG.addNoiseSource(hwRNG);
        rng_initialized = true;
    }
    TEST_ASSERT_FALSE(hwRNG.calibrating());
}

void test_rng_generates_data(void) {
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));

    CryptRNG.rand(buffer, sizeof(buffer));

    // Check that not all bytes are zero
    bool has_nonzero = false;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) {
            has_nonzero = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(has_nonzero);
}

void test_rng_generates_different_data(void) {
    uint8_t buffer1[32];
    uint8_t buffer2[32];

    CryptRNG.rand(buffer1, sizeof(buffer1));
    CryptRNG.rand(buffer2, sizeof(buffer2));

    // Buffers should be different
    TEST_ASSERT_NOT_EQUAL_HEX8_ARRAY(buffer1, buffer2, 32);
}

void test_rng_distribution(void) {
    // Generate 256 bytes and check distribution
    uint8_t buffer[256];
    uint8_t counts[256] = {0};

    CryptRNG.rand(buffer, sizeof(buffer));

    // Count occurrences
    for (size_t i = 0; i < sizeof(buffer); i++) {
        counts[buffer[i]]++;
    }

    // Check for reasonable diversity (at least 64 unique values)
    int unique_values = 0;
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            unique_values++;
        }
    }

    TEST_ASSERT_GREATER_THAN(64, unique_values);
}

void test_rng_available(void) {
    TEST_ASSERT_TRUE(CryptRNG.available(32));
    TEST_ASSERT_TRUE(CryptRNG.available(64));
}

void test_rng_entropy_sources(void) {
    // Print information about ESP32 variant entropy sources
#if defined(CONFIG_IDF_TARGET_ESP32)
    TEST_MESSAGE("ESP32: Thermal noise (ADC) + asynchronous clock mismatch");
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    TEST_MESSAGE("ESP32-S2: SAR ADC + 8 MHz oscillator (always enabled)");
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    TEST_MESSAGE("ESP32-S3: Thermal noise (ADC) + 8 MHz oscillator (always enabled)");
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    TEST_MESSAGE("ESP32-C3: SAR ADC + 8 MHz oscillator (always enabled)");
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    TEST_MESSAGE("ESP32-C6: SAR ADC + 8 MHz oscillator (always enabled)");
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
    TEST_MESSAGE("ESP32-C2: SAR ADC + 8 MHz oscillator (always enabled)");
#else
    TEST_MESSAGE("ESP32: Hardware RNG with thermal noise");
#endif
    TEST_ASSERT_TRUE(true);  // Marker test
}

void test_rng_large_buffer(void) {
    // Test generating a larger buffer
    uint8_t buffer[128];
    CryptRNG.rand(buffer, sizeof(buffer));

    // Check for some level of randomness
    bool has_variation = false;
    for (size_t i = 1; i < sizeof(buffer); i++) {
        if (buffer[i] != buffer[0]) {
            has_variation = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(has_variation);
}

void test_rng_byte_by_byte(void) {
    // Test generating random bytes one at a time
    uint8_t bytes[10];
    for (int i = 0; i < 10; i++) {
        CryptRNG.rand(&bytes[i], 1);
    }

    // Check that not all bytes are the same
    bool all_same = true;
    for (int i = 1; i < 10; i++) {
        if (bytes[i] != bytes[0]) {
            all_same = false;
            break;
        }
    }
    TEST_ASSERT_FALSE(all_same);
}

void setUp(void) {
    // Initialization done in test_rng_initialization
}

void tearDown(void) {
    // Nothing to clean up
}

#ifdef ARDUINO

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_rng_initialization);
    RUN_TEST(test_rng_generates_data);
    RUN_TEST(test_rng_generates_different_data);
    RUN_TEST(test_rng_distribution);
    RUN_TEST(test_rng_available);
    RUN_TEST(test_rng_entropy_sources);
    RUN_TEST(test_rng_large_buffer);
    RUN_TEST(test_rng_byte_by_byte);

    UNITY_END();
}

void loop() {
    // Nothing to do
}

#else

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_rng_initialization);
    RUN_TEST(test_rng_generates_data);
    RUN_TEST(test_rng_generates_different_data);
    RUN_TEST(test_rng_distribution);
    RUN_TEST(test_rng_available);
    RUN_TEST(test_rng_entropy_sources);
    RUN_TEST(test_rng_large_buffer);
    RUN_TEST(test_rng_byte_by_byte);

    return UNITY_END();
}

#endif

#else

// Dummy tests for non-ESP32 platforms
void test_platform_not_supported(void) {
    TEST_IGNORE_MESSAGE("ESP32 RNG tests only run on ESP32 platforms");
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
