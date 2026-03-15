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
