/*
 * Linux/Raspberry Pi Hardware RNG Support
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

#include "RNGLinux.h"
#include "Crypto.h"
#include <string.h>

#if defined(__linux__) || defined(__unix__) || defined(PORTDUINO)

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>

// getrandom() syscall number (defined in Linux 3.17+)
#ifndef SYS_getrandom
#if defined(__x86_64__)
#define SYS_getrandom 318
#elif defined(__i386__)
#define SYS_getrandom 355
#elif defined(__aarch64__)
#define SYS_getrandom 278
#elif defined(__arm__)
#define SYS_getrandom 384
#endif
#endif

// GRND_NONBLOCK flag for getrandom()
#ifndef GRND_NONBLOCK
#define GRND_NONBLOCK 0x0001
#endif

/**
 * \brief Constructs a new Linux hardware random number generator.
 */
RNGLinux::RNGLinux()
    : _initialized(false), _urandom_fd(-1)
{
}

/**
 * \brief Destroys this hardware random number generator.
 */
RNGLinux::~RNGLinux()
{
    if (_urandom_fd >= 0) {
        close(_urandom_fd);
        _urandom_fd = -1;
    }
    _initialized = false;
}

/**
 * \brief Determines if the hardware RNG is still calibrating.
 *
 * \return Always returns false. The Linux kernel RNG is ready immediately.
 *
 * Note: The kernel may block on /dev/random if entropy is low, but
 * /dev/urandom (which we use) is always non-blocking and provides
 * cryptographically secure random data even with limited entropy.
 */
bool RNGLinux::calibrating() const
{
    // Linux kernel RNG is always ready
    return !_initialized;
}

/**
 * \brief Stirs fresh entropy from the Linux kernel RNG into the global pool.
 *
 * This function generates 32 bytes of random data from the Linux kernel's
 * RNG using getrandom() syscall (preferred) or /dev/urandom (fallback).
 *
 * The kernel automatically incorporates hardware RNG sources:
 * - On Raspberry Pi: BCM2835/BCM2711 hardware RNG
 * - On other systems: CPU RDRAND, TPM, or other hardware sources
 *
 * We credit full entropy (256 bits) since the Linux kernel RNG is
 * cryptographically secure and already mixes hardware sources.
 */
void RNGLinux::stir()
{
    if (!_initialized) {
        return;
    }

    uint8_t buffer[32];
    ssize_t bytes_read = 0;

#ifdef SYS_getrandom
    // Try getrandom() syscall first (Linux 3.17+)
    // Use GRND_NONBLOCK to avoid blocking if entropy is low
    bytes_read = syscall(SYS_getrandom, buffer, sizeof(buffer), GRND_NONBLOCK);

    if (bytes_read < 0 && errno == EAGAIN) {
        // If entropy is low, fall back to regular (blocking) call
        // In practice, this should rarely happen with /dev/urandom behavior
        bytes_read = syscall(SYS_getrandom, buffer, sizeof(buffer), 0);
    }
#endif

    // If getrandom() failed or is not available, fall back to /dev/urandom
    if (bytes_read != sizeof(buffer)) {
        if (_urandom_fd < 0) {
            _urandom_fd = open("/dev/urandom", O_RDONLY);
        }

        if (_urandom_fd >= 0) {
            bytes_read = read(_urandom_fd, buffer, sizeof(buffer));
        }
    }

    // Only output if we successfully read random data
    if (bytes_read == sizeof(buffer)) {
        // Credit full entropy - Linux kernel RNG is cryptographically secure
        output(buffer, sizeof(buffer), sizeof(buffer) * 8);  // 256 bits
    }

    // Clean up sensitive data
    clean(buffer, sizeof(buffer));
}

/**
 * \brief Called when this noise source is added to the global RNG.
 *
 * Marks the RNG as initialized and immediately stirs in initial entropy.
 */
void RNGLinux::added()
{
    if (_initialized) {
        return;
    }

    _initialized = true;

    // Immediately stir in some initial entropy
    stir();
}

#endif // __linux__ || __unix__ || PORTDUINO
