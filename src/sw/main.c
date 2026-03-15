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
    if (neorv32_uart0_available() == 0) {  // NOLINT(*)
        return -1;                         // UART0 not available, exit
    }

    /* Initiatet the UART in the 19200 baud rate. */
    neorv32_uart0_setup(BAUD_RATE, 0);  // NOLINT(*)

    /* Ensure that the CFU is available. */
    if (neorv32_cfu_available() == 0) {
        neorv32_uart0_printf(  // NOLINT(*))
            "ERROR! CFU ('Zxcfu' ISA extension) not implemented!\n");
        return -1;
    }

    /* Configure the CFS interrupts. */
    neorv32_rte_handler_install(CFS_TRAP_CODE, &blakes2_hw_irq_handler);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CFS_FIRQ_ENABLE);
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

    compare_performance_of_fletcher();
    compare_performance_of_xor_shift();
    compare_performance_of_blakes2();

    return 0;
}
