/*
 * Test cases for nRF52840 hardware RNG
 */

#include <unity.h>

#if defined(NRF52840_XXAA) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_ITSYBITSY)
#include <RNG.h>
#include <RNGnRF52.h>
#include <string.h>

RNGnRF52 hwRNG;
bool rng_initialized = false;

void test_rng_initialization(void) {
    if (!rng_initialized) {
        CryptRNG.begin("Test Suite");
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

    // Buffers should be different (extremely unlikely to be identical)
    TEST_ASSERT_NOT_EQUAL_HEX8_ARRAY(buffer1, buffer2, 32);
}

void test_rng_distribution(void) {
    // Generate 256 bytes and check that we have reasonable distribution
    uint8_t buffer[256];
    uint8_t counts[256] = {0};

    CryptRNG.rand(buffer, sizeof(buffer));

    // Count occurrences of each byte value
    for (size_t i = 0; i < sizeof(buffer); i++) {
        counts[buffer[i]]++;
    }

    // Check that we have at least some diversity (at least 64 unique values in 256 bytes)
    int unique_values = 0;
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            unique_values++;
        }
    }

    TEST_ASSERT_GREATER_THAN(64, unique_values);
}

void test_rng_available(void) {
    // After generating some data, should have plenty available
    TEST_ASSERT_TRUE(CryptRNG.available(32));
    TEST_ASSERT_TRUE(CryptRNG.available(64));
}

void setUp(void) {
    // Initialization is done in test_rng_initialization
}

void tearDown(void) {
    // Nothing to clean up
}

#ifdef ARDUINO

void setup() {
    delay(2000);  // Wait for serial
    UNITY_BEGIN();

    RUN_TEST(test_rng_initialization);
    RUN_TEST(test_rng_generates_data);
    RUN_TEST(test_rng_generates_different_data);
    RUN_TEST(test_rng_distribution);
    RUN_TEST(test_rng_available);

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

    return UNITY_END();
}

#endif

#else

// Dummy tests for non-nRF52840 platforms
void test_platform_not_supported(void) {
    TEST_IGNORE_MESSAGE("nRF52840 RNG tests only run on nRF52840 hardware");
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

#endif // NRF52840
