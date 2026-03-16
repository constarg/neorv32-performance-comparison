/*
 * File: fletcher32.c
 *
 * Implements the Fletcher, 32-bits, hash function in software.
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
