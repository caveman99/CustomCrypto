/*
 * ESP32 Hardware RNG Support
 * Copyright (C) 2025 caveman99
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
