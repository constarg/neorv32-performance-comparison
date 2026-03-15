#ifndef NEORV32_EXERCISE_BLAKE2_H
#define NEORV32_EXERCISE_BLAKE2_H

#include "neorv32.h"
#include "neorv32_cfs.h"

static inline void
blakes2_hw_irq_handler(void)
{
    NEORV32_CFS->REG[6] = 0x00000000;
}

extern void blake2_sw(void);

extern void blake2_hw(void);

#endif
