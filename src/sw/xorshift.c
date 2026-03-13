#include "xorshift.h"

#include <stdint.h>

uint32_t
xor_shift_sw(uint32_t src_1, uint32_t src_2)
{
    src_1 ^= (src_2 << XOR_SHIFT_13_BITS) ^ (src_2 >> XOR_SHIFT_7_BITS);
    return src_1 * XOR_SHIFT_MULTIPLIER;
}
