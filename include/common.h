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
