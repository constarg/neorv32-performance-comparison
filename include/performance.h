#ifndef NEORV32_EXERCISE_PERFORMANCE_H
#define NEORV32_EXERCISE_PERFORMANCE_H

/* Define the number of times to execute each test. */
#define SUB_ITERATION_1 (1000)
#define SUB_ITERATION_2 (1000)
#define ITERATIONS      (SUB_ITERATION_1 * SUB_ITERATION_2)

extern void compare_performace_of_fletcher(void);

extern void compare_performace_of_xor_shift(void);

#endif
