Custom fork of rweather/Crypto to integrate with STM32 platform RNG

Hardware acceleration:
- ESP32 family (ESP32, S2, S3, C3, C6, C2): AES (all key sizes), hardware RNG
- nRF52840: AES-128, RNG via CC310
- RP2350: SHA-256, TRNG via hardware accelerator
- RP2040: RNG via ROSC (Ring Oscillator)
- STM32WL55: AES-128/256, TRNG via hardware accelerator
- Linux/Raspberry Pi: RNG via kernel (BCM2835/BCM2711 hardware RNG)
