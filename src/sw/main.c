#include <neorv32.h>
#include <stdint.h>

#define KEEP_TWO_LOWER_BYTES (0xFFFF)

#define SHIFT_TWO_BYTES (16)

#define FLETCHER_32_DIVIDER (65536)

#define XOR_SHIFT_MULTIPLIER (2654435761U)

#define XOR_SHIFT_13_BITS (13)
#define XOR_SHIFT_7_BITS  (7)

#define BAUD_RATE (19200)

#define FUNC7_NOT_USED           (0b0000000)
#define FLICHER_OPERATION_OPCODE (0b0101)
#define XOR_MUL_OPERATION_OPCODE (0b0110)

#define FLICHER_OPERATION(SRC_1, SRC_2) \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, FLICHER_OPERATION_OPCODE, SRC_1, SRC_2)

#define XOR_SHIFT_OPERATION(SRC_1, SRC_2) \
    neorv32_cfu_r_instr(FUNC7_NOT_USED, XOR_MUL_OPERATION_OPCODE, SRC_1, SRC_2)

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

uint32_t
xor_shift_sw(uint32_t src_1, uint32_t src_2)
{
    src_1 ^= (src_2 << XOR_SHIFT_13_BITS) ^ (src_2 >> XOR_SHIFT_7_BITS);
    return src_1 * XOR_SHIFT_MULTIPLIER;
}

int
main()
{
    // initialize NEORV32 run-time environment
    neorv32_rte_setup();

    /* Validate that UART is implemented. */
    if (neorv32_uart0_available() == 0) {
        return -1;  // UART0 not available, exit
    }

    // Initiatet the UART in the 19200 baud rate.
    neorv32_uart0_setup(BAUD_RATE, 0);

    // check if the CFU is implemented (the CFU is wrapped in the core's
    // "Zxcfu" ISA extension)
    if (neorv32_cfu_available() == 0) {
        neorv32_uart0_printf(
            "ERROR! CFU ('Zxcfu' ISA extension) not implemented!\n");
        return -1;
    }

    uint32_t result_hw = FLICHER_OPERATION(5, 8);
    neorv32_uart0_printf("HW Flicher result: %d\n", result_hw);

    uint32_t result_sw = fletcher_32_sw(5, 8);
    neorv32_uart0_printf("SW Flicher result: %d\n", result_sw);

    uint32_t result_2_hw = XOR_SHIFT_OPERATION(7, 7);
    neorv32_uart0_printf("HW XOR result: %ul\n", result_2_hw);

    uint32_t result_2_sw = xor_shift_sw(7, 7);
    neorv32_uart0_printf("SW XOR result: %ul\n", result_2_sw);

    return 0;
}
