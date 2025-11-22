/*
 * nRF52840 Hardware RNG Support
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
