#include <neorv32.h>
#include <stdint.h>

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

    compare_performace_of_fletcher();
    compare_performace_of_xor_shift();

    return 0;
}

/*

// I should configure CFS interrupts likewise.

// install DMA interrupt handler
neorv32_rte_handler_install(DMA_TRAP_CODE, dma_firq_handler);

// configure DMA interrupt
neorv32_cpu_csr_set(CSR_MIE, 1 << DMA_FIRQ_ENABLE); // enable DMA interrupt
source neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE); // enable
machine-mode interrupts


neorv32_cpu_sleep();

*/
