/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - ESP32 hardware RNG support
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

#ifndef CRYPTO_RNG_ESP32_h
#define CRYPTO_RNG_ESP32_h

#include "NoiseSource.h"

#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || \
    defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3) || \
    defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C2)

/**
 * \class RNGEsp32 RNGEsp32.h <RNGEsp32.h>
 * \brief Hardware random number generator for ESP32 devices.
 *
 * This class implements a NoiseSource that uses the ESP32's hardware random
 * number generator. The implementation works across all ESP32 variants:
 *
 * - ESP32 (classic): Hardware RNG with thermal noise and clock mismatch
 * - ESP32-S2: Hardware RNG with SAR ADC and 8 MHz oscillator entropy
 * - ESP32-S3: Hardware RNG with thermal noise, ADC, and 8 MHz oscillator
 * - ESP32-C3: Hardware RNG with SAR ADC and 8 MHz oscillator entropy
 * - ESP32-C6: Hardware RNG with SAR ADC and 8 MHz oscillator entropy
 * - ESP32-C2: Hardware RNG with SAR ADC and 8 MHz oscillator entropy
 *
 * The hardware RNG produces true random numbers when the RF subsystem is
 * enabled (i.e., Wi-Fi or Bluetooth is enabled). All variants use SAR ADC
 * for continuous noise mixing, and newer variants (S2, S3, C3, C6, C2) include
 * an additional 8 MHz internal oscillator for entropy.
 *
 * The implementation uses the ESP-IDF functions:
 * - esp_random() for 32-bit random values
 * - esp_fill_random() for bulk random data
 *
 * These functions automatically busy-wait to ensure sufficient entropy
 * has been introduced into the RNG state.
 *
 * Example usage:
 * \code
 * #include <Crypto.h>
 * #include <RNG.h>
 * #include <RNGEsp32.h>
 *
 * RNGEsp32 hwRNG;
 *
 * void setup() {
 *     CryptRNG.begin("MyApp 1.0");
 *     CryptRNG.addNoiseSource(hwRNG);
 * }
 *
 * void loop() {
 *     uint8_t random_data[32];
 *     CryptRNG.rand(random_data, sizeof(random_data));
 *     // Use random_data...
 * }
 * \endcode
 *
 * References:
 * - https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/random.html
 * - https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-reference/system/random.html
 * - https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/random.html
 * - https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/system/random.html
 * - https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/system/random.html
 */
class RNGEsp32 : public NoiseSource
{
public:
    RNGEsp32();
    ~RNGEsp32();

    bool calibrating() const;
    void stir();
    void added();

private:
    bool _initialized;
};

#endif // ESP32 variants

#endif // CRYPTO_RNG_ESP32_h
