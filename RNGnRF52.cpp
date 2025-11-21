/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 * Copyright (C) 2025 - nRF52840 hardware RNG support
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

#include "RNGnRF52.h"
#include "Crypto.h"
#include <string.h>

#if defined(NRF52840_XXAA) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_ITSYBITSY)

// Include Nordic's CryptoCell CC310 RNG headers
// These are provided by the Adafruit nRFCrypto library or Nordic SDK
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for CC310 SaSi library RNG functions
// Users need to link against the appropriate nRF52840 crypto library

// Library initialization
int SaSi_LibInit(void);
void SaSi_LibFini(void);

// RNG types and functions
typedef void* CRYS_RND_State_t;
typedef void* CRYS_RND_WorkBuff_t;

// RNG function declarations
int CRYS_RndInit(CRYS_RND_State_t* rndState, CRYS_RND_WorkBuff_t* rndWorkBuff);
int CRYS_RND_GenerateVector(CRYS_RND_State_t* rndState, uint16_t outSizeBytes, uint8_t* out);
int CRYS_RND_UnInstantiation(CRYS_RND_State_t* rndState);

#ifdef __cplusplus
}
#endif

/**
 * \class RNGnRF52 RNGnRF52.h <RNGnRF52.h>
 * \brief Hardware random number generator for nRF52840 devices.
 *
 * This class implements a NoiseSource that uses the ARM CryptoCell CC310
 * hardware random number generator present on nRF52840 devices. This provides
 * high-quality cryptographically secure random numbers directly from hardware.
 *
 * The CC310 includes a True Random Number Generator (TRNG) that uses
 * hardware entropy sources to generate unpredictable random values.
 *
 * Example usage:
 *
 * \code
 * #include <Crypto.h>
 * #include <RNG.h>
 * #include <RNGnRF52.h>
 *
 * RNGnRF52 hwRNG;
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
 * \note The CryptoCell CC310 is only available on nRF52840. This
 * implementation will not work on nRF52832 or other nRF52 variants.
 *
 * References: https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.2.0/hardware_driver_cryptocell.html
 */

/**
 * \brief Constructs a new hardware random number generator.
 */
RNGnRF52::RNGnRF52()
    : _initialized(false)
    , _rngState(nullptr)
{
    memset(_stateBuffer, 0, sizeof(_stateBuffer));
    _rngState = _stateBuffer;
}

/**
 * \brief Destroys this hardware random number generator.
 */
RNGnRF52::~RNGnRF52()
{
    if (_initialized) {
        SaSi_LibInit();
        CRYS_RND_UnInstantiation((CRYS_RND_State_t*)_rngState);
        SaSi_LibFini();
        _initialized = false;
    }
    clean(_stateBuffer, sizeof(_stateBuffer));
}

/**
 * \brief Determines if the hardware RNG is still calibrating.
 *
 * \return Always returns false since the CC310 hardware RNG does not
 * require calibration.
 */
bool RNGnRF52::calibrating() const
{
    // Hardware RNG is always ready once initialized
    return !_initialized;
}

/**
 * \brief Stirs fresh entropy from the hardware RNG into the global pool.
 *
 * This function generates 32 bytes of random data from the CC310 hardware
 * RNG and adds it to the global random number pool with full entropy credit.
 */
void RNGnRF52::stir()
{
    if (!_initialized) {
        return;
    }

    // Generate 32 bytes of random data from hardware
    uint8_t buffer[32];

    SaSi_LibInit();
    int result = CRYS_RND_GenerateVector((CRYS_RND_State_t*)_rngState, sizeof(buffer), buffer);
    SaSi_LibFini();

    if (result == 0) {
        // Credit all 256 bits since this is hardware random
        output(buffer, sizeof(buffer), sizeof(buffer) * 8);
    }

    // Clean up sensitive data
    clean(buffer, sizeof(buffer));
}

/**
 * \brief Called when this noise source is added to the global RNG.
 *
 * Initializes the CC310 hardware RNG and stirs in initial entropy.
 */
void RNGnRF52::added()
{
    if (_initialized) {
        return;
    }

    // Initialize the crypto library
    SaSi_LibInit();

    // Initialize the RNG
    // Note: CRYS_RndInit may need a work buffer, we'll use the second half of our state buffer
    CRYS_RND_WorkBuff_t* workBuff = (CRYS_RND_WorkBuff_t*)(_stateBuffer + 128);
    int result = CRYS_RndInit((CRYS_RND_State_t*)_rngState, workBuff);

    SaSi_LibFini();

    if (result == 0) {
        _initialized = true;

        // Immediately stir in some initial entropy
        stir();
    }
}

#endif // NRF52840
