# Test Suite

This directory contains test cases for the Crypto library.

## Running Tests

### PlatformIO

Tests are automatically discovered by PlatformIO. Run with:

```bash
pio test -e esp32          # For ESP32 tests
pio test -e esp32s2        # For ESP32-S2 tests
pio test -e esp32s3        # For ESP32-S3 tests
pio test -e esp32c3        # For ESP32-C3 tests
pio test -e nrf52840       # For nRF52840 tests
pio test -e pico           # For RP2040 tests
pio test -e pico2          # For RP2350 tests
pio test -e stm32wl55      # For STM32WL55 tests
pio test -e linux          # For Linux/Raspberry Pi tests
pio test                   # For all platforms
```

### Arduino IDE

Tests can be compiled as regular Arduino sketches. Open the test file in Arduino IDE and upload to your board.

## Test Files

### ESP32 Family Tests

- `test_esp32_aes.cpp` - Hardware AES tests (all ESP32 variants)
  - Tests AES-128/192/256 hardware acceleration
  - Tests CTR mode operation
  - Validates key size handling
  - Uses NIST test vectors
  - Supports ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-C2

- `test_esp32_rng.cpp` - Hardware RNG tests (all ESP32 variants)
  - Tests hardware RNG initialization
  - Tests random data generation
  - Tests data uniqueness and distribution
  - Displays variant-specific entropy sources
  - Supports ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-C2

### nRF52840 Tests

- `test_nrf52_aes.cpp` - AES encryption/decryption tests (all platforms)
  - Tests AES-128 hardware acceleration on nRF52840
  - Tests AES-192/256 software implementation
  - Tests CTR mode operation
  - Validates key size handling

- `test_nrf52_rng.cpp` - Hardware RNG tests (nRF52840 only)
  - Tests RNG initialization
  - Tests random data generation
  - Tests data uniqueness and distribution
  - nRF52840 only (skipped on other platforms)

### RP2040/RP2350 Tests

- `test_pico_rng.cpp` - Hardware RNG tests (RP2040/RP2350)
  - Tests RNG initialization and availability
  - Tests random data generation and uniqueness
  - Tests distribution quality
  - Works on both RP2040 (ROSC) and RP2350 (TRNG)

- `test_pico_sha256.cpp` - Hardware SHA-256 tests (RP2350 only)
  - Tests SHA-256 with NIST test vectors
  - Tests incremental updates
  - Tests multiple block handling
  - Tests HMAC functionality
  - RP2350 only (skipped on RP2040)

### STM32WL55 Tests

- `test_stm32_aes.cpp` - Hardware AES tests (STM32WL55)
  - Tests AES-128 hardware acceleration
  - Tests AES-256 hardware acceleration
  - Tests CTR mode operation
  - Validates key size handling
  - Uses NIST test vectors

- `test_stm32_rng.cpp` - Hardware RNG tests (STM32WL55)
  - Tests TRNG initialization
  - Tests random data generation
  - Tests data uniqueness and distribution
  - STM32WL55 only (skipped on other platforms)

### Linux/Raspberry Pi Tests

- `test_linux_rng.cpp` - Kernel RNG tests (Linux)
  - Tests Linux kernel RNG initialization
  - Tests random data generation via getrandom()/urandom
  - Tests data uniqueness and distribution
  - Automatically uses hardware RNG (BCM2835/BCM2711 on Raspberry Pi)
  - Linux only (skipped on other platforms)

## Test Vectors

Test cases use NIST standardized test vectors to ensure correct implementation.

## Coverage

Tests cover:
- **ESP32 family** (ESP32, S2, S3, C3, C6, C2): Hardware AES-128/192/256, Hardware RNG
- **nRF52840**: Hardware AES-128, Software AES-192/256, Hardware RNG
- **RP2350**: Hardware SHA-256, Hardware TRNG
- **RP2040**: ROSC-based RNG
- **STM32WL55**: Hardware AES-128/256, Hardware TRNG
- **Linux/Raspberry Pi**: Kernel RNG (BCM2835/BCM2711 hardware RNG)
- Cipher mode operations (CTR)
- Key validation and data integrity
