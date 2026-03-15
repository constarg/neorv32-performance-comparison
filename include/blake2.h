/* file: blake2.h */
#ifndef NEORV32_EXERCISE_BLAKE2_H
#define NEORV32_EXERCISE_BLAKE2_H

#include "neorv32.h"
#include "neorv32_cfs.h"

/* Indicates the CFS register offset in which the v[a] is to be stored. */
#define CFS_ADDRESS_OF_V_A    (0)
/* Indicates the CFS register offset in which the v[b] is to be stored. */
#define CFS_ADDRESS_OF_V_B    (1)
/* Indicates the CFS register offset in which the v[c] is to be stored. */
#define CFS_ADDRESS_OF_V_C    (2)
/* Indicates the CFS register offset in which the v[d] is to be stored. */
#define CFS_ADDRESS_OF_V_D    (3)
/* Indicates the CFS register offset in which the X is to be stored. */
#define CFS_ADDRESS_OF_X      (4)
/* Indicates the CFS register offset in which the Y is to be stored. */
#define CFS_ADDRESS_OF_Y      (5)
/* Indicates the CFS register offset in which the status of the CO-Processor
 * is to be stored. */
#define CFS_ADDRESS_OF_STATUS (6)

/* CFS_INIT_WORD Is used to initiate the calculations on the side of
 * the CO-Processor. After initialization, the CO-Processor will announse
 * that the results are ready to be read via an interrupt assertion. */
#define CFS_INIT_WORD (0x0000000F)

/* CFS_FINISHED_WORD Is used to stop the calculations on the side of
 * the CO-Processor. If this word is not written the CO-Processor
 * will continuously assert an CFS interrupt!. */
#define CFS_FINISHED_WORD (0x00000000)

/* CFS_START_WORD Is used to start the hash calculations. */
#define CFS_START_WORD (CFS_INIT_WORD)

/* BLAKES_ARRAY_SIZE Define the number of elements in V[i]. */
#define BLAKES_ARRAY_SIZE (16)

/**
 * blakes2_hw_irq_handler Is called when a CFS interrupt is asserted. When this
 * interrupt is asserted, it means that the CO Processor have finished the
 * calculations of BLAKES2 Algorithm, and the data are ready to be read.
 */
static inline void
blakes2_hw_irq_handler(void)
{
    NEORV32_CFS->REG[CFS_ADDRESS_OF_STATUS] = CFS_FINISHED_WORD;  // NOLINT(*)
}

/**
 * blakes2_hw_start_processing Is levereged to provide the required singal to
 * the NEORV32 soft processor, CFS CO-Processor to start processing the BLAKES2
 * algorithms, based on the provided array of data. This function should be
 * called after all the data required for the calculation of the hash function
 * are transmitted to the CO-Processor's memory.
 */
static inline void
blakes2_hw_start_processing(void)
{
    NEORV32_CFS->REG[CFS_ADDRESS_OF_STATUS] = CFS_START_WORD;  // NOLINT(*)
}

/* blakes2_sw Implements the blakes2 hash function in software. */
extern uint32_t* blake2_sw(void);

/* blakes2_hw Executes the accelerated blakes2 hash function in hardware. */
extern uint32_t* blake2_hw(void);

#endif
