-- ================================================================================ --
-- NEORV32 SoC - Custom Functions Subsystem (CFS)                                   --
-- -------------------------------------------------------------------------------- --
-- The NEORV32 RISC-V Processor - https://github.com/stnolting/neorv32              --
-- Copyright (c) NEORV32 contributors.                                              --
-- Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  --
-- Licensed under the BSD-3-Clause license, see LICENSE for details.                --
-- SPDX-License-Identifier: BSD-3-Clause                                            --
-- ================================================================================ --

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library neorv32;
use neorv32.neorv32_package.all;

entity neorv32_cfs is
  port (
    -- global control --
    clk_i     : in  std_ulogic; -- global clock line
    rstn_i    : in  std_ulogic; -- global reset line, low-active, async
    -- CPU access --
    bus_req_i : in  bus_req_t; -- bus request
    bus_rsp_o : out bus_rsp_t; -- bus response
    -- CPU interrupt --
    irq_o     : out std_ulogic; -- interrupt request
    -- external IO --
    cfs_in_i  : in  std_ulogic_vector(255 downto 0); -- custom inputs conduit
    cfs_out_o : out std_ulogic_vector(255 downto 0) -- custom outputs conduit
  );
end neorv32_cfs;

architecture neorv32_cfs_rtl of neorv32_cfs is

    constant CFS_ADDRESS_OF_V_A : INTEGER := 0;
    constant CFS_ADDRESS_OF_V_B : INTEGER := 1;
    constant CFS_ADDRESS_OF_V_C : INTEGER := 2;
    constant CFS_ADDRESS_OF_V_D : INTEGER := 3;
    constant CFS_ADDRESS_OF_X   : INTEGER := 4;
    constant CFS_ADDRESS_OF_Y   : INTEGER := 5;

    constant CFS_ADDRESS_OF_STATUS : INTEGER := 6;

    type cfs_regs_t is array (0 to 5) of std_ulogic_vector(31 downto 0); 
    signal cfs_reg_wr : cfs_regs_t; -- for WRITE accesses
    signal cfs_reg_rd : cfs_regs_t; -- for READ accesses

    type blake_step_t is record 
        opa : STD_ULOGIC_VECTOR(31 downto 0);
        opb : STD_ULOGIC_VECTOR(31 downto 0);
        opc : STD_ULOGIC_VECTOR(31 downto 0);
        res : STD_ULOGIC_VECTOR(31 downto 0);
    end record; 
begin

    cfs_out_o <= (others => '0'); -- not used.

    -- irq_o <= '0'; 
  
    bus_access: process(rstn_i, clk_i)
        variable addr_index : INTEGER; 
        begin
            if (rstn_i = '0') then
                cfs_reg_wr <= (others => (others => '0'));
                bus_rsp_o  <= rsp_terminate_c;
            elsif rising_edge(clk_i) then 
                -- transfer/access acknowledge --
                bus_rsp_o.ack <= bus_req_i.stb; -- send ACK right after the access request
                bus_rsp_o.err <= '0'; -- set high together with bus_rsp_o.ack if there is an access error

                -- bus access --
                bus_rsp_o.data <= (others => '0'); -- the output HAS TO BE ZERO if there is no actual (read) access
                if (bus_req_i.stb = '1') then -- valid access cycle, STB is high for exactly one cycle
                    addr_index := to_integer(unsigned(bus_req_i.addr(15 downto 2)));

                    if (addr_index < cfs_reg_wr'length) then
                        cfs_reg_wr(addr_index) <= bus_req_i.data;
                    end if;

                -- read access (word-wise) --
                else
                    if (addr_index < cfs_reg_rd'length) then
                        bus_rsp_o.data <= cfs_reg_rd(addr_index);
                    end if;
                end if;
            end if;
    end process bus_access;

    blake2_process: process(rstn_i, clk_i)
        variable phase : INTEGER;
        
        variable v_a : blake_step_t; 
        variable v_b : blake_step_t;
        variable v_c : blake_step_t;
        variable v_d : blake_step_t;

        -- Phase 1. 
        variable v_a_add_v_b_add_x : UNSIGNED(31 downto 0);
        variable v_a_add_v_b_add_y : UNSIGNED(31 downto 0);
        variable v_c_add_v_d : UNSIGNED(31 downto 0);
   
        begin
            if (rstn_i = '0') then
                phase := 0;
            elsif rising_edge(clk_i) then
                if (cfs_reg_wr(CFS_ADDRESS_OF_STATUS) = x"0000000F") then 
                    case phase is
                        when 0 =>
                            v_a.opa    := cfs_reg_wr(CFS_ADDRESS_OF_V_A);
                            v_a.opb    := cfs_reg_wr(CFS_ADDRESS_OF_V_B);
                            v_a.opc    := cfs_reg_wr(CFS_ADDRESS_OF_X);
                            v_a.res    := (others => '0');

                            v_b.opa    := (others => '0');
                            v_b.opb    := (others => '0');
                            v_b.opc    := (others => '0');
                            v_b.res    := (others => '0');

                            v_c.opa    := cfs_reg_wr(CFS_ADDRESS_OF_V_C);
                            v_c.opb    := cfs_reg_wr(CFS_ADDRESS_OF_V_D);
                            v_c.opc    := (others => '0');
                            v_c.res    := (others => '0');

                            v_d.opa    := (others => '0');
                            v_d.opb    := (others => '0');
                            v_d.opc    := (others => '0');
                            v_d.res    := (others => '0');

                            v_a_add_v_b_add_x := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc);
                            v_a.res := STD_ULOGIC_VECTOR(v_a_add_v_b_add_x);
            
                            v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);
                            v_c.res := STD_ULOGIC_VECTOR(v_c_add_v_d);
    
                            phase := 1;
                        when 1 => 
                            v_d.opa := cfs_reg_wr(CFS_ADDRESS_OF_V_D) xor v_a.res;
                            v_b.opa := cfs_reg_wr(CFS_ADDRESS_OF_V_B) xor v_c.res;
    
                            -- perform the ROT.
                            -- X >> 16 | x << 16
                            v_d.res := v_d.opa(15 downto 0) & v_d.opa(31 downto 16);

                            -- X << 12 | x >> 12
                            v_b.res := v_b.opa(11 downto 0) & v_b.opa(31 downto 12);
                            phase := 2;

                        when 2 => 
                            v_a.opa := v_a.res;
                            v_a.opb := v_b.res;
                            v_a.opc := cfs_reg_wr(CFS_ADDRESS_OF_Y);  

                            v_a_add_v_b_add_y := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc);

                            v_a.res := STD_ULOGIC_VECTOR(v_a_add_v_b_add_y);
                            phase := 3;
                        when 3 => 
                            v_d.opa := v_d.res xor v_a.res;
                            v_d.res := v_d.opa(7 downto 0) & v_d.opa(31 downto 8); 

                            phase := 4;
                        when 4 =>
                            v_c.opa := v_c.res;
                            v_c.opb := v_d.res;

                            v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);

                            v_c.res := STD_ULOGIC_VECTOR(v_c_add_v_d);
                            phase := 5;
                        when 5 => 
                            v_b.opa := v_b.res xor v_c.res;

                            v_b.res := v_b.opa(6 downto 0) & v_b.opa(31 downto 7);
                            phase := 6;
                        when 6 =>
                            cfs_reg_rd(CFS_ADDRESS_OF_V_A) <= v_a.res;
                            cfs_reg_rd(CFS_ADDRESS_OF_V_B) <= v_b.res;
                            cfs_reg_rd(CFS_ADDRESS_OF_V_C) <= v_c.res;
                            cfs_reg_rd(CFS_ADDRESS_OF_V_D) <= v_d.res; 
                            irq_o <= '1';
                        when others => phase := 0;
                    end case;
                else 
                    irq_o <= '0';
                end if;
            end if;

    end process blake2_process;

end neorv32_cfs_rtl;
