#include <neorv32.h>
#include <stdint.h>

#include "blake2.h"
#include "common.h"
#include "neorv32_cpu.h"
#include "performance.h"

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

    neorv32_rte_handler_install(CFS_TRAP_CODE, &blakes2_hw_irq_handler);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CFS_FIRQ_ENABLE);
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

    // compare_performace_of_fletcher();
    // compare_performace_of_xor_shift();

    // uint32_t v[16];
    // /* Deterministic initial values */
    // for (int i = 0; i < 16; i++) { v[i] = 0x11111111u * (uint32_t)(i + 1); }
    //
    // uint32_t p[16];
    // for (int i = 0; i < 16; i++) { p[i] = v[i]; }
    //
    // uint32_t x = 0xAAAAAAAAu;
    // uint32_t y = 0x55555555u;
    //
    // neorv32_uart0_printf(" Before G () :\n ");
    // for (int i = 0; i < 16; i++) {
    //     neorv32_uart0_printf("v[%2d] = 0x%08" PRIx32 "\n", i, v[i]);
    // }
    // neorv32_uart0_printf("\nApplying G(v,0,4,8,12,x,y)\n\n");
    // G(v, 0, 4, 8, 12, x, y);
    //
    // neorv32_uart0_printf("After G() FOR V:\n");
    // for (int i = 0; i < 16; i++) {
    //     neorv32_uart0_printf("v[%d] = %d\n", i, v[i]);
    // }
    //
    // G_hw(p, 0, 4, 8, 12, x, y);
    //
    // neorv32_uart0_printf("After G() FOR P:\n");
    // for (int i = 0; i < 16; i++) {
    //     neorv32_uart0_printf("p[%d] = %d\n", i, p[i]);
    // }

    return 0;
}
