// SPDX-License-Identifier: CC0-1.0 OR Zlib
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdbool.h>
#include <stdint.h>
#include <nds.h>
#include "r4tf.h"

#define ROMCTRL_FLAGS (CARD_SEC_CMD | CARD_SEC_DAT | CARD_SEC_EN | CARD_DELAY1(0) | CARD_DELAY2(24))

static uint32_t get_status(void) {
    REG_AUXSPICNTH = CARD_CR1_ENABLE | CARD_CR1_IRQ;
    REG_CARD_COMMAND[0] = CMD_GET_STATUS;
    REG_ROMCTRL = ROMCTRL_FLAGS | CARD_BLK_SIZE(7) | CARD_nRESET | CARD_ACTIVATE;
    while (!(REG_ROMCTRL & CARD_DATA_READY));
    return REG_CARD_DATA_RD;
}

// Initialize the driver. Returns true on success.
bool startup(void) {
    return (get_status() & STATUS_INIT_MASK) != STATUS_INIT_IN_PROGRESS;
}

// Returns true if a card is present and initialized.
bool is_inserted(void) {
    return (get_status() & STATUS_INIT_MASK) == STATUS_INIT_SUCCESS;
}

// Clear error flags from the card. Returns true on success.
bool clear_status(void) {
    return true;
}

static inline void prepare_sector_in_command(uint32_t sector) {
    REG_CARD_COMMAND[1] = sector >> 24;
    REG_CARD_COMMAND[2] = sector >> 16;
    REG_CARD_COMMAND[3] = sector >> 8;
    // write low byte to byte 4 + clear bytes 5-7
    *((uint32_t*) (REG_CARD_COMMAND + 4)) = sector & 0xFF;
}

// Reads 512 byte sectors into a buffer that may be unaligned. Returns true on
// success.
bool read_sectors(uint32_t sector, uint32_t num_sectors, uint8_t *buffer) {
    uint8_t wordbuf[4] __attribute__((aligned(4)));

    if (sector >= 0x800000)
        return false;
    sector <<= 9;

    REG_AUXSPICNTH = CARD_CR1_ENABLE | CARD_CR1_IRQ;
    while (num_sectors) {
        REG_CARD_COMMAND[0] = CMD_SD_READ_REQUEST;
        prepare_sector_in_command(sector);
        do {
            REG_ROMCTRL = ROMCTRL_FLAGS | CARD_BLK_SIZE(7) | CARD_nRESET | CARD_ACTIVATE;
            while (!(REG_ROMCTRL & CARD_DATA_READY));
        } while (REG_CARD_DATA_RD != 0);

        REG_CARD_COMMAND[0] = CMD_SD_READ_DATA;
        REG_ROMCTRL = ROMCTRL_FLAGS | CARD_BLK_SIZE(1) | CARD_nRESET | CARD_ACTIVATE;
        sector += 512; num_sectors--;

        if ((uint32_t)buffer & 3) {
            do {
                if (REG_ROMCTRL & CARD_DATA_READY) {
                    *((uint32_t*) wordbuf) = REG_CARD_DATA_RD;

                    *(buffer++) = wordbuf[0];
                    *(buffer++) = wordbuf[1];
                    *(buffer++) = wordbuf[2];
                    *(buffer++) = wordbuf[3];
                }
            } while (REG_ROMCTRL & CARD_BUSY);
        } else {
            do {
                if (REG_ROMCTRL & CARD_DATA_READY) {
                    *((uint32_t*) buffer) = REG_CARD_DATA_RD;
                    buffer += 4;
                }
            } while (REG_ROMCTRL & CARD_BUSY);
        }
    }

    return true;
}

// Writes 512 byte sectors from a buffer that may be unaligned. Returns true on
// success.
bool write_sectors(uint32_t sector, uint32_t num_sectors, const uint8_t *buffer) {
    uint8_t wordbuf[4] __attribute__((aligned(4)));

    if (sector >= 0x800000)
        return false;
    sector <<= 9;

    REG_AUXSPICNTH = CARD_CR1_ENABLE | CARD_CR1_IRQ;
    while (num_sectors) {
        REG_CARD_COMMAND[0] = CMD_SD_WRITE_START;
        prepare_sector_in_command(sector);
        REG_ROMCTRL = ROMCTRL_FLAGS | CARD_BLK_SIZE(1) | CARD_WR | CARD_nRESET | CARD_ACTIVATE;
        if ((uint32_t)buffer & 3) {
            for (uint32_t i = 0; i < 128; i++) {
                wordbuf[0] = *(buffer++);
                wordbuf[1] = *(buffer++);
                wordbuf[2] = *(buffer++);
                wordbuf[3] = *(buffer++);
                while (!(REG_ROMCTRL & CARD_DATA_READY));
                REG_CARD_DATA_RD = *wordbuf;
            }
        } else {
            for (uint32_t i = 0; i < 128; i++) {
                while (!(REG_ROMCTRL & CARD_DATA_READY));
                REG_CARD_DATA_RD = *((uint32_t*) buffer);
                buffer += 4;
            }
        }

        REG_CARD_COMMAND[0] = CMD_SD_WRITE_STATUS;
        do {
            REG_ROMCTRL = ROMCTRL_FLAGS | CARD_BLK_SIZE(7) | CARD_nRESET | CARD_ACTIVATE;
            while (!(REG_ROMCTRL & CARD_DATA_READY));
        } while (REG_CARD_DATA_RD != 0);

        sector += 512; num_sectors--;
    }

    return true;
}

// Shutdowns the card. This may never be called.
bool shutdown(void) {
    return true;
}
