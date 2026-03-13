#include "performance.h"

#include <neorv32.h>
#include <stdint.h>

#include "fetcher32.h"
#include "xorshift.h"

void
compare_performace_of_fletcher(void)
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
            result            = FLETCHER_OPERATION(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }

    neorv32_uart0_printf("Hardware Fletcher Implementation result: %d\n",
                         result);
    neorv32_uart0_printf(
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

    neorv32_uart0_printf("Software Fletcher Implementation result: %d\n",
                         result);
    neorv32_uart0_printf(
        "Software Fletcher Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);
}

void
compare_performace_of_xor_shift(void)
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
            result            = XOR_SHIFT_OPERATION(i, j);
            cpu_cycles_after  = neorv32_cpu_get_cycle();
            cpu_cycles_took   = cpu_cycles_after - cpu_cycles_before;
            cpu_total_cycles += cpu_cycles_took;
        }
    }
    neorv32_uart0_printf("Hardware XOR Shift Implementation result: %d\n",
                         result);

    neorv32_uart0_printf(
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

    neorv32_uart0_printf("Software XOR Shift Implementation result: %d\n",
                         result);

    neorv32_uart0_printf(
        "Software XOR Shift Implementation took: %ul mean cycles\n",
        cpu_total_cycles / (long)ITERATIONS);
}
