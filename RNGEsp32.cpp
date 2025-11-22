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

#include "RNGEsp32.h"
#include "Crypto.h"
#include <string.h>

#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || \
    defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3) || \
    defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C2)

// Declare ESP-IDF RNG functions
extern "C" {
uint32_t esp_random(void);
void esp_fill_random(void *buf, size_t len);
}

/**
 * \brief Constructs a new hardware random number generator.
 */
RNGEsp32::RNGEsp32()
    : _initialized(false)
{
}

/**
 * \brief Destroys this hardware random number generator.
 */
RNGEsp32::~RNGEsp32()
{
    _initialized = false;
}

/**
 * \brief Determines if the hardware RNG is still calibrating.
 *
 * \return Always returns false. The ESP32 hardware RNG is ready immediately
 * after initialization and automatically busy-waits to ensure sufficient
 * entropy.
 *
 * Note: The RNG produces true random numbers when the RF subsystem is enabled
 * (Wi-Fi or Bluetooth). Even without RF, newer variants (S2, S3, C3, C6, C2)
 * have a secondary entropy source from an 8 MHz internal oscillator that is
 * always enabled.
 */
bool RNGEsp32::calibrating() const
{
    // ESP32 hardware RNG is ready once we've initialized
    return !_initialized;
}

/**
 * \brief Stirs fresh entropy from the hardware RNG into the global pool.
 *
 * This function generates 32 bytes of random data from the ESP32's hardware
 * RNG using esp_fill_random() and adds it to the global random number pool.
 *
 * Entropy sources vary by ESP32 variant:
 * - ESP32: Thermal noise (ADC) + asynchronous clock mismatch
 * - ESP32-S2: SAR ADC noise + 8 MHz oscillator (always enabled)
 * - ESP32-S3: Thermal noise (ADC) + 8 MHz oscillator (always enabled)
 * - ESP32-C3: SAR ADC noise + 8 MHz oscillator (always enabled)
 * - ESP32-C6: SAR ADC noise + 8 MHz oscillator (always enabled)
 * - ESP32-C2: SAR ADC noise + 8 MHz oscillator (always enabled)
 *
 * The hardware RNG automatically busy-waits to ensure sufficient external
 * entropy has been introduced, providing true random numbers suitable for
 * cryptographic operations.
 *
 * We credit full entropy (256 bits) since the ESP32 hardware RNG is
 * cryptographically secure.
 */
void RNGEsp32::stir()
{
    if (!_initialized) {
        return;
    }

    // Generate 32 bytes of random data using ESP-IDF RNG
    uint8_t buffer[32];
    esp_fill_random(buffer, sizeof(buffer));

    // Credit full entropy - ESP32 hardware RNG is cryptographically secure
    // The hardware automatically ensures sufficient entropy is mixed in
    output(buffer, sizeof(buffer), sizeof(buffer) * 8);  // 256 bits

    // Clean up sensitive data
    clean(buffer, sizeof(buffer));
}

/**
 * \brief Called when this noise source is added to the global RNG.
 *
 * Marks the RNG as initialized and immediately stirs in initial entropy.
 */
void RNGEsp32::added()
{
    if (_initialized) {
        return;
    }

    _initialized = true;

    // Immediately stir in some initial entropy
    stir();
}

#endif // ESP32 variants
