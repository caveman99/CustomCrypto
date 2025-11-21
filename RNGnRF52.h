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

#ifndef CRYPTO_RNG_NRF52_h
#define CRYPTO_RNG_NRF52_h

#include "NoiseSource.h"

#if defined(NRF52840_XXAA) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_ITSYBITSY)

class RNGnRF52 : public NoiseSource
{
public:
    RNGnRF52();
    ~RNGnRF52();

    bool calibrating() const;
    void stir();
    void added();

private:
    bool _initialized;
    void* _rngState;  // Opaque pointer to CRYS_RND_State_t
    uint8_t _stateBuffer[256];  // Enough space for the RNG state
};

#endif // NRF52840

#endif // CRYPTO_RNG_NRF52_h
