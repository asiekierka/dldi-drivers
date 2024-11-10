#ifndef _CARD_H_
#define _CARD_H_

#include <stdint.h>
#include <nds.h>

__attribute__((always_inline))
static inline void card_enable(void) {
    REG_AUXSPICNTH = CARD_CR1_ENABLE | CARD_CR1_IRQ;
}

void card_read_aligned(void *buffer);
void card_read(uint8_t *buffer);

__attribute__((always_inline))
static inline uint32_t card_read4(void) {
    while (!(REG_ROMCTRL & CARD_DATA_READY));
    return REG_CARD_DATA_RD;
}

void card_write_aligned(const void *buffer);
void card_write(const uint8_t *buffer);

#endif /* _CARD_H_ */
