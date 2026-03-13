#include "fetcher32.h"

#include <stdint.h>

#include "common.h"

uint32_t
fletcher_32_sw(uint32_t src_1, uint32_t src_2)
{
    uint16_t partial_1 = (src_1 & KEEP_TWO_LOWER_BYTES)
                         + ((src_1 >> SHIFT_TWO_BYTES) & KEEP_TWO_LOWER_BYTES);

    uint16_t partial_2 = (partial_1 + (src_2 & KEEP_TWO_LOWER_BYTES)
                          + ((src_2 >> SHIFT_TWO_BYTES)
                             & KEEP_TWO_LOWER_BYTES))
                         % FLETCHER_32_DIVIDER;

    return (partial_2 << SHIFT_TWO_BYTES) | partial_1;
}
