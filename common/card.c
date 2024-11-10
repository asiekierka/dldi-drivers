// SPDX-License-Identifier: CC0-1.0 OR Zlib
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdbool.h>
#include <stdint.h>
#include <nds.h>

void card_read_aligned(void *buffer) {
    do {
        if (REG_ROMCTRL & CARD_DATA_READY) {
            *((uint32_t*) buffer) = REG_CARD_DATA_RD;
            buffer += 4;
        }
    } while (REG_ROMCTRL & CARD_BUSY);
}

void card_read(uint8_t *buffer) {
    uint8_t wordbuf[4] __attribute__((aligned(4)));

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

void card_write_aligned(const void *buffer) {
    for (uint32_t i = 0; i < 128; i++) {
        while (!(REG_ROMCTRL & CARD_DATA_READY));
        REG_CARD_DATA_RD = *((uint32_t*) buffer);
        buffer += 4;
    }
}


void card_write(const uint8_t *buffer) {
    uint8_t wordbuf[4] __attribute__((aligned(4)));

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
}

