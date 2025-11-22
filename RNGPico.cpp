/*
 * RP2040/RP2350 Hardware RNG Support
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

#include "RNGPico.h"
#include "Crypto.h"
#include <string.h>

#if defined(PICO_BOARD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO)

// Include Pico SDK headers
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for Pico SDK functions
// These are provided by the pico-sdk's pico_rand library
uint32_t get_rand_32(void);
uint64_t get_rand_64(void);

#ifdef __cplusplus
}
#endif

/**
 * \brief Constructs a new hardware random number generator.
 */
RNGPico::RNGPico()
    : _initialized(false)
{
}

/**
 * \brief Destroys this hardware random number generator.
 */
RNGPico::~RNGPico()
{
    _initialized = false;
}

/**
 * \brief Determines if the hardware RNG is still calibrating.
 *
 * \return Always returns false. The Pico SDK's RNG is ready immediately
 * after initialization.
 *
 * Note: On RP2040, the ROSC-based RNG may have lower quality initially but
 * is always "ready". On RP2350, the hardware TRNG is ready once initialized.
 */
bool RNGPico::calibrating() const
{
    // The Pico SDK RNG is ready once we've initialized
    return !_initialized;
}

/**
 * \brief Stirs fresh entropy from the hardware RNG into the global pool.
 *
 * This function generates 32 bytes of random data from the Pico SDK's
 * get_rand_32() function and adds it to the global random number pool.
 *
 * On RP2040: Uses ROSC (Ring Oscillator) as entropy source
 * On RP2350: Uses hardware TRNG for high-quality randomness
 *
 * Entropy credit:
 * - RP2040: Credit 16 bits per 32-bit word (ROSC has limited entropy)
 * - RP2350: Credit full 32 bits per word (hardware TRNG)
 */
void RNGPico::stir()
{
    if (!_initialized) {
        return;
    }

    // Generate 32 bytes of random data (8 x 32-bit words)
    uint8_t buffer[32];
    uint32_t *words = (uint32_t*)buffer;

    for (int i = 0; i < 8; i++) {
        words[i] = get_rand_32();
    }

    // Credit entropy based on platform
    // RP2350 has hardware TRNG, RP2040 has ROSC (lower quality)
    // We conservatively credit 16 bits per 32-bit word for RP2040
    // and full 32 bits for RP2350
#if defined(RP2350A) || defined(RP2350B)
    // RP2350: Hardware TRNG - credit full entropy
    unsigned int credit_bits = sizeof(buffer) * 8;  // 256 bits
#else
    // RP2040: ROSC-based - credit half entropy conservatively
    unsigned int credit_bits = sizeof(buffer) * 4;  // 128 bits
#endif

    output(buffer, sizeof(buffer), credit_bits);

    // Clean up sensitive data
    clean(buffer, sizeof(buffer));
}

/**
 * \brief Called when this noise source is added to the global RNG.
 *
 * Marks the RNG as initialized and immediately stirs in initial entropy.
 */
void RNGPico::added()
{
    if (_initialized) {
        return;
    }

    _initialized = true;

    // Immediately stir in some initial entropy
    stir();
}

#endif // PICO_BOARD || ARDUINO_ARCH_RP2040
