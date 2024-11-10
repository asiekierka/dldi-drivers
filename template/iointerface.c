// SPDX-License-Identifier: CC0-1.0 OR Zlib
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdbool.h>
#include <stdint.h>
#include <nds.h>

// Initialize the driver. Returns true on success.
bool startup(void) {
    // TODO
    return false;
}

// Returns true if a card is present and initialized.
bool is_inserted(void) {
    // TODO
    return false;
}

// Clear error flags from the card. Returns true on success.
bool clear_status(void) {
    return true;
}

// Reads 512 byte sectors into a buffer that may be unaligned. Returns true on
// success.
bool read_sectors(uint32_t sector, uint32_t num_sectors, uint8_t *buffer) {
    // TODO
    return false;
}

// Writes 512 byte sectors from a buffer that may be unaligned. Returns true on
// success.
bool write_sectors(uint32_t sector, uint32_t num_sectors, const uint8_t *buffer) {
    // TODO
    return false;
}

// Shutdowns the card. This may never be called.
bool shutdown(void) {
    return true;
}
