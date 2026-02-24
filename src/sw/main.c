#include <neorv32.h>
#include <stdint.h>

#include "neorv32_cpu.h"

/* This macro is levereged to keep only the
 * two lower bytes of an number. */
#define KEEP_TWO_LOWER_BYTES (0xFFFF)

/* This macro is levereged for shifting
 * a number 2 bytes to some direction. */
#define SHIFT_TWO_BYTES (16)

/* This macro is a predetermined numerical value
 * which is used as a devider during the
 * computation of the Fletcher Hash fucntion. */
#define FLETCHER_32_DIVIDER (65536)

/* This macro is a predetermined numberical value
 * which is used as a multiplier during the
 * computation of the XOR Shift Hash function. */
#define XOR_SHIFT_MULTIPLIER (2654435761U)

/* This macro is used to shift 13 bits to some
 * direction. */
#define XOR_SHIFT_13_BITS (13)
/* This macro is used to shift 7 bits to some
 * direction. */
#define XOR_SHIFT_7_BITS  (7)

/* This macro determines the BAUD RATE of the
 * UART. */
#define BAUD_RATE (19200)

/* FUNC7_NOT_USED indicates that the FUNC7 bit-field
 * of the NEORV32 instruction bits aren't used. */
#define FUNC7_NOT_USED             (0b0000000)
/* FLETCHER_OPERATION_OPCODE specifies which code is
 * to be set in the FUNC3 bit-field, in order to execute
 * the Fletcher Hash function on hardware. */
#define FLETCHER_OPERATION_OPCODE  (0b0101)
/* XOR_MUL_OPERATION_OPCODE specifies which code is
 * to be set in the FUNC3 bit-field, in order to execute
 * the XOR Shift Hash function. */
#define XOR_SHIFT_OPERATION_OPCODE (0b0110)

/* FLETCHER_OPERATION simplifies the process of calling the
 * hardware implemented Fletcher Hash function. */
#define FLETCHER_OPERATION(SRC_1, SRC_2)                                  \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, FLETCHER_OPERATION_OPCODE, SRC_1, \
                        SRC_2)

/* XOR_SHIFT_OPERATION simplifies the process of calling the
 * hardware implemented XOR Shift Hash function. */
#define XOR_SHIFT_OPERATION(SRC_1, SRC_2)                                  \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, XOR_SHIFT_OPERATION_OPCODE, SRC_1, \
                        SRC_2)

/* Define the number of times to execute each test. */
#define SUB_ITERATION_1 (1000)
#define SUB_ITERATION_2 (1000)
#define ITERATIONS      (SUB_ITERATION_1 * SUB_ITERATION_2)

/**
 * fletcher_32_sw Implements the fletcher Hash function from the software
 * end-point.
 *
 * @param src_1 The first block of data.
 * @param src_2 The second block of data.
 */
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

/**
 * xor_shift_sw Implements the XOR Hash function from the software end-point.
 *
 * @param src_1 The first block of data.
 * @param src_2 The second block of data.
 */
uint32_t
xor_shift_sw(uint32_t src_1, uint32_t src_2)
{
    src_1 ^= (src_2 << XOR_SHIFT_13_BITS) ^ (src_2 >> XOR_SHIFT_7_BITS);
    return src_1 * XOR_SHIFT_MULTIPLIER;
}

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

int
main(void)
{
    /* initialize NEORV32 run-time environment */
    neorv32_rte_setup();

    /* Validate that UART is implemented. */
    if (neorv32_uart0_available() == 0) {
        return -1;  // UART0 not available, exit
    }

    /* Initiatet the UART in the 19200 baud rate. */
    neorv32_uart0_setup(BAUD_RATE, 0);

    /* Ensure that the CFU is available. */
    if (neorv32_cfu_available() == 0) {
        neorv32_uart0_printf(
            "ERROR! CFU ('Zxcfu' ISA extension) not implemented!\n");
        return -1;
    }

    compare_performace_of_fletcher();
    compare_performace_of_xor_shift();

    return 0;
}
