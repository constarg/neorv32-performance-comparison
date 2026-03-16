/**
 * File: performance.h
 *
 ***********************************************************************
 * Contains function definitions for comparing the performance
 * of software implementation of various hash functions against
 * their hardware implementation.
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
 ***********************************************************************
 */
#ifndef NEORV32_EXERCISE_PERFORMANCE_H
#define NEORV32_EXERCISE_PERFORMANCE_H

/* Define the number of times to execute each test. */
#define SUB_ITERATION_1 (100)
#define SUB_ITERATION_2 (100)
#define ITERATIONS      (SUB_ITERATION_1 * SUB_ITERATION_2)

/**
 * comapre_performance_of_fletcher Executes ITERATION times
 * the fletcher algorithm in both hardware and software
 * and compares the performance.
 */
extern void compare_performance_of_fletcher(void);

/**
 * compare_performance_of_xor_shift Executes ITERATION times
 * the xor_shift algorithm in both hardware and software
 * and compares the performance.
 */
extern void compare_performance_of_xor_shift(void);

/**
 * compare_performance_of_blakes2 Executes ITERATION times
 * the blakes2 hash algorithm in both hardware and software
 * and compares the performance.
 */
extern void compare_performance_of_blakes2(void);

#endif
