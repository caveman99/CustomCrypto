/*
 * Test cases for RP2040/RP2350 hardware RNG
 */

#include <unity.h>

#if defined(PICO_BOARD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO)
#include <RNG.h>
#include <RNGPico.h>
#include <string.h>

RNGPico hwRNG;
bool rng_initialized = false;

void test_rng_initialization(void) {
    if (!rng_initialized) {
        CryptRNG.begin("Pico Test Suite");
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

void test_rng_quality_indicator(void) {
    // On RP2350, we should get higher quality randomness
    // On RP2040, ROSC-based is lower quality but still functional
#if defined(RP2350A) || defined(RP2350B)
    // RP2350 has hardware TRNG
    TEST_MESSAGE("Running on RP2350 with hardware TRNG");
#else
    // RP2040 has ROSC-based RNG
    TEST_MESSAGE("Running on RP2040 with ROSC-based RNG");
#endif
    TEST_ASSERT_TRUE(true);  // Just a marker test
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
    RUN_TEST(test_rng_quality_indicator);

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
    RUN_TEST(test_rng_quality_indicator);

    return UNITY_END();
}

#endif

#else

// Dummy tests for non-Pico platforms
void test_platform_not_supported(void) {
    TEST_IGNORE_MESSAGE("Pico RNG tests only run on RP2040/RP2350 hardware");
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

#endif // PICO_BOARD
