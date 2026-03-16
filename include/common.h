/*
 * File: common.h
 *
 * Contains common MACROS across the codebase..
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
#ifndef NEORV32_EXERCISE_COMMON_H
#define NEORV32_EXERCISE_COMMON_H

/* This macro is levereged to keep only the
 * two lower bytes of an number. */
#define KEEP_TWO_LOWER_BYTES (0xFFFF)

/* This macro is levereged for shifting
 * a number 2 bytes to some direction. */
#define SHIFT_TWO_BYTES (16)

/* This macro determines the BAUD RATE of the
 * UART. */
#define BAUD_RATE (19200)

/* FUNC7_NOT_USED indicates that the FUNC7 bit-field
 * of the NEORV32 instruction bits aren't used. */
#define FUNC7_NOT_USED (0b0000000)

#endif
