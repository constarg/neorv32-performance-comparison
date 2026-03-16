/*
 * File: xorshift.c
 *
 * Implements the XORShift hash function in software.
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
#include "xorshift.h"

#include <stdint.h>

uint32_t
xor_shift_sw(uint32_t src_1, uint32_t src_2)
{
    src_1 ^= (src_2 << XOR_SHIFT_13_BITS) ^ (src_2 >> XOR_SHIFT_7_BITS);
    return src_1 * XOR_SHIFT_MULTIPLIER;
}
