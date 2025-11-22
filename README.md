# Crypto

Custom fork of rweather/Crypto to add hardware random number generator (RNG) support for multiple platforms.

## Hardware RNG Support

- ESP32 family (ESP32, S2, S3, C3, C6, C2): Hardware RNG
- nRF52840: RNG via CC310 cryptographic accelerator
- RP2350: TRNG via hardware accelerator
- RP2040: RNG via ROSC (Ring Oscillator)
- STM32WL55: TRNG via hardware accelerator
- Linux/Raspberry Pi: RNG via kernel (BCM2835/BCM2711 hardware RNG)

## License

This library is derived from the original [rweather/Crypto](https://github.com/rweather/arduinolibs) library, which is licensed under the MIT License.

**Hardware RNG extensions** added by caveman99 are licensed under the **GNU General Public License v3.0 (GPLv3)**.

**Important**: When using this library with the hardware RNG extensions, the combined work is subject to the terms of the GPLv3 license. If you use only the original MIT-licensed components without the hardware RNG extensions, the MIT license applies.
