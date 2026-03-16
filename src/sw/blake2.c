/*
 * File: blakes2.c
 *
 * Implements the software for the blakes2 hash function, and invokes
 * the blakes2 function which is implemented in the hardware layer.
 *
 * Copyright (C) 2026  Constantinos Argyriou
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
 *
 * Email: constarg@pm.me
 */
#include "blake2.h"

#include <stdint.h>

#include "neorv32.h"
#include "neorv32_cfs.h"

#define X_VALUE (0xAAAAAAAAu)
#define Y_VALUE (0x55555555u)

#define V_CONTENT (0x11111111u)

// NOLINTBEGIN(readability-magic-numbers)
static inline uint32_t
rotr32(uint32_t x, unsigned n)  // NOLINT(*)
{
    return (x >> n) | (x << (32U - n));
}

/* G Impelements the G function in software. */
static inline void
G(uint32_t v[16], int a, int b, int c, int d, uint32_t x,  // NOLINT(*)
  uint32_t y)                                              // NOLINT(*)
{
    v[a] = v[a] + v[b] + x;
    v[d] = rotr32(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = rotr32(v[b] ^ v[c], 12);
    v[a] = v[a] + v[b] + y;
    v[d] = rotr32(v[d] ^ v[a], 8);
    v[c] = v[c] + v[d];
    v[b] = rotr32(v[b] ^ v[c], 7);
}

/* G_hw Executes the accelerated version of G, in the CO-Processor. */
static inline void
G_hw(uint32_t v[16], int a, int b, int c, int d, uint32_t x,  // NOLINT(*)
     uint32_t y)                                              // NOLINT(*)
{
    /* Write the required data into the CO-Processor's memory. */
    NEORV32_CFS->REG[CFS_ADDRESS_OF_V_A] = v[a];  // NOLINT(*)
    NEORV32_CFS->REG[CFS_ADDRESS_OF_V_B] = v[b];  // NOLINT(*)
    NEORV32_CFS->REG[CFS_ADDRESS_OF_V_C] = v[c];  // NOLINT(*)
    NEORV32_CFS->REG[CFS_ADDRESS_OF_V_D] = v[d];  // NOLINT(*)

    NEORV32_CFS->REG[CFS_ADDRESS_OF_X] = x;  // NOLINT(*)
    NEORV32_CFS->REG[CFS_ADDRESS_OF_Y] = y;  // NOLINT(*)

    blakes2_hw_start_processing();

    /* Wait for the CO-Processor to assert an CFS interrupt. */
    neorv32_cpu_sleep();

    /* Read the result. */
    v[a] = NEORV32_CFS->REG[CFS_ADDRESS_OF_V_A];  // NOLINT(*)
    v[b] = NEORV32_CFS->REG[CFS_ADDRESS_OF_V_B];  // NOLINT(*)
    v[c] = NEORV32_CFS->REG[CFS_ADDRESS_OF_V_C];  // NOLINT(*)
    v[d] = NEORV32_CFS->REG[CFS_ADDRESS_OF_V_D];  // NOLINT(*)
}

uint32_t*
blake2_sw(void)
{
    static uint32_t v_sw[BLAKES_ARRAY_SIZE];  // NOLINT(*)
    uint32_t        x = X_VALUE;              // NOLINT(*)
    uint32_t        y = Y_VALUE;              // NOLINT(*)

    /* Deterministic initial values */
    for (int i = 0; i < BLAKES_ARRAY_SIZE; i++) {
        v_sw[i] = V_CONTENT * (uint32_t)(i + 1);
    }

    G(v_sw, 0, 4, 8, 12, x, y);

    return v_sw;
}

uint32_t*
blake2_hw(void)
{
    static uint32_t v_hw[BLAKES_ARRAY_SIZE];  // NOLINT(*)
    uint32_t        x = X_VALUE;              // NOLINT(*)
    uint32_t        y = Y_VALUE;              // NOLINT(*)

    /* Deterministic initial values */
    for (int i = 0; i < BLAKES_ARRAY_SIZE; i++) {
        v_hw[i] = V_CONTENT * (uint32_t)(i + 1);
    }

    G_hw(v_hw, 0, 4, 8, 12, x, y);

    return v_hw;
}

// NOLINTEND(readability-magic-numbers)
