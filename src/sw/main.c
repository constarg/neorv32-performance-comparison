/**
 * File: main.c
 *
 ***********************************************************************
 * Copyright (C) 2026  Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Email: constarg@pm.me
 ***********************************************************************
 */
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
