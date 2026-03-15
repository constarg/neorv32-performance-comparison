#include "blake2.h"

#include <stdint.h>

#include "neorv32.h"
#include "neorv32_cfs.h"

// NOLINTBEGIN(readability-magic-numbers)
static inline uint32_t
rotr32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32U - n));
}
static inline void
G(uint32_t v[16], int a, int b, int c, int d, uint32_t x, uint32_t y)
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

static inline void
G_hw(uint32_t v[16], int a, int b, int c, int d, uint32_t x, uint32_t y)
{
    NEORV32_CFS->REG[0] = v[a];  // NOLINT(*)
    NEORV32_CFS->REG[1] = v[b];  // NOLINT(*)
    NEORV32_CFS->REG[2] = v[c];  // NOLINT(*)
    NEORV32_CFS->REG[3] = v[d];  // NOLINT(*)

    NEORV32_CFS->REG[4] = x;           // NOLINT(*)
    NEORV32_CFS->REG[5] = y;           // NOLINT(*)
    NEORV32_CFS->REG[6] = 0x0000000F;  // NOLINT(*)

    neorv32_cpu_sleep();

    v[a] = NEORV32_CFS->REG[0];  // NOLINT(*)
    v[b] = NEORV32_CFS->REG[1];  // NOLINT(*)
    v[c] = NEORV32_CFS->REG[2];  // NOLINT(*)
    v[d] = NEORV32_CFS->REG[3];  // NOLINT(*)
}

void
blake2_sw(void)
{
    uint32_t v[16];
    uint32_t x = 0xAAAAAAAAu;
    uint32_t y = 0x55555555u;

    /* Deterministic initial values */
    for (int i = 0; i < 16; i++) { v[i] = 0x11111111u * (uint32_t)(i + 1); }

    G(v, 0, 4, 8, 12, x, y);
}

void
blake2_hw(void)
{
    uint32_t v[16];
    uint32_t x = 0xAAAAAAAAu;
    uint32_t y = 0x55555555u;

    /* Deterministic initial values */
    for (int i = 0; i < 16; i++) { v[i] = 0x11111111u * (uint32_t)(i + 1); }

    G_hw(v, 0, 4, 8, 12, x, y);
}

// NOLINTEND(readability-magic-numbers)
