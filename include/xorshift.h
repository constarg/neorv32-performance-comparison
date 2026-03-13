#ifndef NEORV32_EXERCISE_XORSHIFT_H
#define NEORV32_EXERCISE_XORSHIFT_H

#include <stdint.h>

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

/* XOR_MUL_OPERATION_OPCODE specifies which code is
 * to be set in the FUNC3 bit-field, in order to execute
 * the XOR Shift Hash function. */
#define XOR_SHIFT_OPERATION_OPCODE (0b0110)

/* XOR_SHIFT_OPERATION simplifies the process of calling the
 * hardware implemented XOR Shift Hash function. */
#define XOR_SHIFT_OPERATION(SRC_1, SRC_2)                                  \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, XOR_SHIFT_OPERATION_OPCODE, SRC_1, \
                        SRC_2)

/**
 * xor_shift_sw Implements the XOR Hash function from the software end-point.
 *
 * @param src_1 The first block of data.
 * @param src_2 The second block of data.
 */
extern uint32_t xor_shift_sw(uint32_t src_1, uint32_t src_2);

#endif
