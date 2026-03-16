/*
 * File: fletcher32.h
 *
 * Contains MACROS and function definition for the implementation of
 * fletcher, 32-bits, hash function in both software and hardware.
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
#ifndef NEORV32_EXERCISE_FETCHER32_H
#define NEORV32_EXERCISE_FETCHER32_H

#include <stdint.h>

#include "common.h"

/* This macro is a predetermined numerical value
 * which is used as a devider during the
 * computation of the Fletcher Hash fucntion. */
#define FLETCHER_32_DIVIDER (65536)

/* FLETCHER_OPERATION_OPCODE specifies which code is
 * to be set in the FUNC3 bit-field, in order to execute
 * the Fletcher Hash function on hardware. */
#define FLETCHER_OPERATION_OPCODE (0b0101)

/* FLETCHER_OPERATION simplifies the process of calling the
 * hardware implemented Fletcher Hash function. */
#define FLETCHER_OPERATION_HW(SRC_1, SRC_2)                               \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, FLETCHER_OPERATION_OPCODE, SRC_1, \
                        SRC_2)

/**
 * fletcher_32_sw Implements the fletcher Hash function from the software
 * end-point.
 *
 * @param src_1 The first block of data.
 * @param src_2 The second block of data.
 */
extern uint32_t fletcher_32_sw(uint32_t src_1, uint32_t src_2);

#endif
