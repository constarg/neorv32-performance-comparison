/*
 * File: performance.c
 *
 * Implements the functions to compare the performance of hash functions
 * implemented in software against the one's implemented in hardware.
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
#include "performance.h"

#include <neorv32.h>
#include <stdint.h>

#include "blake2.h"
#include "fetcher32.h"
#include "neorv32_uart.h"
#include "xorshift.h"

void
compare_performance_of_fletcher(void)
{
    volatile uint64_t cpu_cycles_before; /* Represents the CPU cycles passed,
                                   from boot, before an operation. */
    volatile uint64_t cpu_cycles_after;  /* Represents the CPU cycles passed,
                                   from boot, after an operation is
                                   finished. */
    uint64_t          cpu_cycles_took;   /* Represents the total CPU cycles an
                                            operation took to be done. */
    uint64_t          cpu_total_cycles;  /* Represents the total number of CPU
                                            cycles count in every iteration. */

    volatile uint32_t result;

    cpu_total_cycles = 0;

    /* Execute the Fletcher Hash function in both the hardware
       and the software and measure the CPU cycles took for
       both to finish. */

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = FLETCHER_OPERATION_HW(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware Fletcher Implementation result: %d\n", result);
    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware Fletcher Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);

    cpu_total_cycles = 0;

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = fletcher_32_sw(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf(  // NOLINT(*)
        "Software Fletcher Implementation result: %d\n", result);
    neorv32_uart0_printf(  // NOLINT(*)
        "Software Fletcher Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);
}

void
compare_performance_of_xor_shift(void)
{
    volatile uint64_t cpu_cycles_before; /* Represents the CPU cycles passed,
                                   from boot, before an operation. */
    volatile uint64_t cpu_cycles_after;  /* Represents the CPU cycles passed,
                                   from boot, after an operation is
                                   finished. */
    uint64_t          cpu_cycles_took;   /* Represents the total CPU cycles an
                                            operation took to be done. */
    uint64_t          cpu_total_cycles;  /* Represents the total number of CPU
                                            cycles count in every iteration. */

    volatile uint32_t result;

    cpu_total_cycles = 0;

    /* Execute the XOR Shift Hash function in both the hardware
       and the software and measure the CPU cycles took for
       both to finish. */

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = XOR_SHIFT_OPERATION_HW(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }
    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware XOR Shift Implementation result: %d\n", result);

    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware XOR Shift Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);

    cpu_total_cycles = 0;

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = xor_shift_sw(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf(  // NOLINT(*)
        "Software XOR Shift Implementation result: %d\n", result);

    neorv32_uart0_printf(  // NOLINT(*)
        "Software XOR Shift Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);
}

void
compare_performance_of_blakes2(void)
{
    volatile uint64_t cpu_cycles_before; /* Represents the CPU cycles passed,
                                   from boot, before an operation. */
    volatile uint64_t cpu_cycles_after;  /* Represents the CPU cycles passed,
                                   from boot, after an operation is
                                   finished. */
    uint64_t          cpu_cycles_took;   /* Represents the total CPU cycles an
                                            operation took to be done. */
    uint64_t          cpu_total_cycles;  /* Represents the total number of CPU
                                            cycles count in every iteration. */

    volatile uint32_t *result;

    cpu_total_cycles = 0;

    /* Execute the BLAKES2 Hash function in both the hardware
       and the software and measure the CPU cycles took for
       both to finish. */

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = blake2_hw();
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware BLAKES2 Implementation result: ");

    for (int res = 0; res < BLAKES_ARRAY_SIZE; res++) {
        neorv32_uart0_printf("%d, ", result[res]);  // NOLINT(*)
    }
    neorv32_uart0_printf("\n");  // NOLINT(*)

    neorv32_uart0_printf(  // NOLINT(*)
        "Hardware BLAKES2 Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);

    cpu_total_cycles = 0;

    for (int i = 0; i < SUB_ITERATION_1; i++) {
        for (int j = 0; j < SUB_ITERATION_2; j++) {
            cpu_cycles_before = neorv32_cpu_get_cycle();
            result            = blake2_sw();
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf(  // NOLINT(*)
        "Software BLAKES2 Implementation result: ");

    for (int res = 0; res < BLAKES_ARRAY_SIZE; res++) {
        neorv32_uart0_printf("%d, ", result[res]);  // NOLINT(*)
    }
    neorv32_uart0_printf("\n");  // NOLINT(*)

    neorv32_uart0_printf(  // NOLINT(*)
        "Software BLAKES2 Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);
}
