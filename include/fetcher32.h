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
