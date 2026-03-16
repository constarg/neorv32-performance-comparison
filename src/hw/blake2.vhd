----------------------------------------------------------------------------------
--
-- File: blakes2.vhd
--
------------------------------------------------------------------
-- Implements the blake's G function in hardware using the Custom 
-- Function Subsystem (CFS).
--
-- Copyright (C) 2026  Constantinos Argyriou
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.
--
-- Email: constarg@pm.me
------------------------------------------------------------------
--
-- Engineer:  Constantinos Argyriou
-- 
-- Create Date: 02/23/2026 11:59:47 AM
-- Design Name: Blakes2 Hash functions accelerator.  
-- Module Name: neorv32_cfs - Behavioral
-- Project Name: MSc - Embedded Systems design Exercise 
-- Target Devices:  XC7A35T 
-- Tool Versions:  2023.2
-- Description:  Implements a hardware accelerator for the Blakes2 Hash function.
-- 
-- Dependencies: Neorv32 source. 
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library neorv32;
use neorv32.neorv32_package.all;

entity neorv32_cfs is
  port (
    -- global control --
    clk_i     : in  STD_ULOGIC; -- global clock line
    rstn_i    : in  STD_ULOGIC; -- global reset line, low-active, async
    -- CPU access --
    bus_req_i : in  bus_req_t; -- bus request
    bus_rsp_o : out bus_rsp_t; -- bus response
    -- CPU interrupt --
    irq_o     : out STD_ULOGIC; -- interrupt request
    -- external IO --
    cfs_in_i  : in  STD_ULOGIC_VECTOR(255 downto 0); -- custom inputs conduit
    cfs_out_o : out STD_ULOGIC_VECTOR(255 downto 0) -- custom outputs conduit
  );
end neorv32_cfs;

architecture neorv32_cfs_rtl of neorv32_cfs is
    -- Indicates the CFS register offset in which the v[a] is to be stored.
    constant CFS_ADDRESS_OF_V_A : INTEGER := 0;
    -- Indicates the CFS register offset in which the v[b] is to be stored.
    constant CFS_ADDRESS_OF_V_B : INTEGER := 1;
    -- Indicates the CFS register offset in which the v[c] is to be stored.
    constant CFS_ADDRESS_OF_V_C : INTEGER := 2;
    -- Indicates the CFS register offset in which the v[d] is to be stored.
    constant CFS_ADDRESS_OF_V_D : INTEGER := 3;
    -- Indicates the CFS register offset in which the X is to be stored.
    constant CFS_ADDRESS_OF_X   : INTEGER := 4;
    -- Indicates the CFS register offset in which the Y is to be stored.
    constant CFS_ADDRESS_OF_Y   : INTEGER := 5;

    -- Indicates the CFS register offset from which the CFS status is read.
    constant CFS_ADDRESS_OF_STATUS : INTEGER := 6;

    -- Define the write only register type.
    type cfs_wr_regs_t is array (0 to 6) of STD_ULOGIC_VECTOR(31 downto 0);
    -- Define the read only registers type.
    type cfs_rd_regs_t is array (0 to 3) of STD_ULOGIC_VECTOR(31 downto 0); 

    signal cfs_reg_wr : cfs_wr_regs_t; -- for WRITE accesses
    signal cfs_reg_rd : cfs_rd_regs_t; -- for READ accesses

    -- blake_step_t Is levereged for keeping the state of blakes algorithsm
    -- steps. Each blakes step have AT MOST 3 operands.
    type blake_step_t is record 
        -- The first operand for the next operation. 
        opa : STD_ULOGIC_VECTOR(31 downto 0);
        -- The second operand for the next operation.
        opb : STD_ULOGIC_VECTOR(31 downto 0);
        -- The third operand for the next operation. 
        opc : STD_ULOGIC_VECTOR(31 downto 0);
        -- The result from the previous calculation (or the last result).
        res : STD_ULOGIC_VECTOR(31 downto 0);
    end record; 
    
    -- Indicates the calculation phase of the BLAKE STATE MACHINE.
    signal phase : INTEGER;
    
    -- Define the following to keep track of each distinct variable.
    signal v_a : blake_step_t; 
    signal v_b : blake_step_t;
    signal v_c : blake_step_t;
    signal v_d : blake_step_t;

begin
    cfs_out_o <= (others => '0'); -- not used for this minimal example

    bus_access: process(rstn_i, clk_i)
    begin
        if (rstn_i = '0') then
            cfs_reg_wr(0) <= (others => '0');
            cfs_reg_wr(1) <= (others => '0');
            cfs_reg_wr(2) <= (others => '0');
            cfs_reg_wr(3) <= (others => '0');
            cfs_reg_wr(4) <= (others => '0');
            cfs_reg_wr(5) <= (others => '0');
            cfs_reg_wr(6) <= (others => '0');

            bus_rsp_o     <= rsp_terminate_c;
        elsif rising_edge(clk_i) then -- synchronous interface for read and write accesses
            -- transfer/access acknowledge --
            bus_rsp_o.ack <= bus_req_i.stb; -- send ACK right after the access request
            bus_rsp_o.err <= '0'; -- set high together with bus_rsp_o.ack if there is an access error

            -- bus access --
            bus_rsp_o.data <= (others => '0'); -- the output HAS TO BE ZERO if there is no actual (read) access
            if (bus_req_i.stb = '1') then -- valid access cycle, STB is high for exactly one cycle

                -- write access (word-wise) --
                if (bus_req_i.rw = '1') then
                    if (bus_req_i.addr(15 downto 2) = "00000000000000") then -- 16-bit byte address = 14-bit word address
                        cfs_reg_wr(0) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000001") then
                        cfs_reg_wr(1) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000010") then
                        cfs_reg_wr(2) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000011") then
                        cfs_reg_wr(3) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000100") then
                        cfs_reg_wr(4) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000101") then
                        cfs_reg_wr(5) <= bus_req_i.data;
                    end if;
                    if (bus_req_i.addr(15 downto 2) = "00000000000110") then
                        cfs_reg_wr(6) <= bus_req_i.data;
                    end if;

                -- read access (word-wise) --
                else
                    case bus_req_i.addr(15 downto 2) is -- 16-bit byte address = 14-bit word address
                        when "00000000000000" => bus_rsp_o.data <= cfs_reg_rd(0);
                        when "00000000000001" => bus_rsp_o.data <= cfs_reg_rd(1);
                        when "00000000000010" => bus_rsp_o.data <= cfs_reg_rd(2);
                        when "00000000000011" => bus_rsp_o.data <= cfs_reg_rd(3);
                        when others           => bus_rsp_o.data <= (others => '0');
                    end case;
                end if;
            end if;
        end if;
    end process bus_access;

    -- The state machine below, performes the BLAKES2 algorithm.
    blakes_steps: process(clk_i)
    -- Holds the result of v[a] + v[b] + x. 
    variable add_v_a_add_v_b_add_x : UNSIGNED(31 downto 0);
    -- Holds the result of v[c] + v[d].
    variable add_v_c_add_v_d : UNSIGNED(31 downto 0);
    -- Holds the result of v[a] + v[b] + y.
    variable add_v_a_add_v_b_add_y: UNSIGNED(31 downto 0);
    -- Holds the result of rotr32(v[d] ^ v[a], 16).
    variable v_d_rot_16 : STD_ULOGIC_VECTOR(31 downto 0);
    -- Holds the result of rotr32(v[b] ^ v[c], 12).
    variable v_b_rot_12 : STD_ULOGIC_VECTOR(31 downto 0);
    -- Holds the result of rotr32(v[d] ^ v[a], 8).
    variable v_d_rot_8: STD_ULOGIC_VECTOR(31 downto 0);
    begin
        if (rising_edge(clk_i)) then 
            if (cfs_reg_wr(CFS_ADDRESS_OF_STATUS) = x"0000000F") then 
                case phase is 
                    when 0 => 
                        -- Load the operands for the first operation. 
                        -- Operation to be done: v[a] + v[b] + x.
                        v_a.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_A);
                        v_a.opb <= cfs_reg_wr(CFS_ADDRESS_OF_V_B);
                        v_a.opc <= cfs_reg_wr(CFS_ADDRESS_OF_X);
                        phase <= 1;
                    when 1 =>
                        -- Perform the v[a] + v[b] + x.
                        add_v_a_add_v_b_add_x := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc);
                        -- Store the first result of v[a].
                        v_a.res <= STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_x);

                        -- Prepare the operands for the rotr32(v[d] ^ v[a], 16).
                        -- Calculate the v[d] ^ v[a].
                        v_d.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_D) xor STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_x);
                        phase <= 2;
                    when 2 =>
                        -- Perform the rotr32(v[d] ^ v[a], 16).
                        v_d_rot_16 := v_d.opa(15 downto 0) & v_d.opa(31 downto 16);
                        -- Store the first result of v[d].
                        v_d.res <= v_d_rot_16;

                        -- Prepare the operands for the v[c] + v[d].
                        v_c.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_C);
                        v_c.opb <= v_d_rot_16;
                        phase <= 3;
                    when 3 => 
                        -- Perform the v[c] + v[d] operation. 
                        add_v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);
                        -- Store the first result of v[c].
                        v_c.res <= STD_ULOGIC_VECTOR(add_v_c_add_v_d);

                        -- Prepare the operands for the rotr32(v[b] ^ v[c], 12).
                        v_b.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_B) xor STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        phase <= 4;
                    when 4 => 
                        -- Perform the operation rotr32(v[b] ^ v[c], 12).
                        v_b_rot_12 := v_b.opa(11 downto 0) & v_b.opa(31 downto 12);
                        -- Store the result to v[b].
                        v_b.res <= v_b_rot_12;

                        -- Prepare the operands for the v[a] + v[b] + y.
                        v_a.opa <= v_a.res;
                        v_a.opb <= v_b_rot_12;
                        v_a.opc <= cfs_reg_wr(CFS_ADDRESS_OF_Y);
                        phase <= 5;
                    when 5 =>
                        -- Perform the operation v[a] + v[b] + y.
                        add_v_a_add_v_b_add_y := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc);
                        -- Store the new result in v[a].
                        v_a.res <= STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_y);

                        -- Prepare the operands for the rotr32(v[d] ^ v[a], 8)
                        v_d.opa <= v_d.res xor STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_y);
                        phase <= 6;
                    when 6 =>
                        -- Perform the operation rotr32(v[d] ^ v[a], 8).
                        v_d_rot_8 := v_d.opa(7 downto 0) & v_d.opa(31 downto 8);
                        -- Store the result in v[d].
                        v_d.res <= v_d_rot_8;

                        -- Prepare the operands for the v[c] + v[d].
                        v_c.opa <= v_c.res;
                        v_c.opb <= v_d_rot_8;
                        phase <= 7;
                    when 7 =>
                        -- Perform the operation v[c] + v[d].
                        add_v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);
                        -- Store the result in v[c].
                        v_c.res <= STD_ULOGIC_VECTOR(add_v_c_add_v_d);

                        -- Prepare the operands for the last operation rotr32(v[b] ^ v[c], 7).
                        v_b.opa <= v_b.res xor STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        phase <= 8;
                    when 8 =>
                        -- Peform the operation rotr32(v[b] ^ v[c], 7).
                        v_b.res <= v_b.opa(6 downto 0) & v_b.opa(31 downto 7);
                        irq_o <= '1';
                        phase <= 9;
                    when 9 => 
                        -- WAIT in this phase until the software side handles the interrupt 
                        -- assertion.
                        if (cfs_reg_wr(CFS_ADDRESS_OF_STATUS) = x"00000000") then 
                            phase <= 0;
                            irq_o <= '0';
                        end if;
                    when others => phase <= 0; 
                end case; 
            end if;

        end if;
    end process blakes_steps;

-- Update the READ ONLY register, for the software to obtain 
-- the results.
cfs_reg_rd(CFS_ADDRESS_OF_V_A) <= v_a.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_B) <= v_b.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_C) <= v_c.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_D) <= v_d.res;

end neorv32_cfs_rtl;
