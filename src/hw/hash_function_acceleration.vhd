----------------------------------------------------------------------------------
-- Engineer:  Constantinos Argyriou
-- 
-- Create Date: 02/23/2026 11:59:47 AM
-- Design Name:  Hash functions accelerator.  
-- Module Name: hash_function_acceleration - Behavioral
-- Project Name: MSc - Embedded Systems design Exercise 
-- Target Devices:  XC7A35T 
-- Tool Versions:  2023.2
-- Description:  Implements a hardware accelerator for two hash functions. The first 
-- is the Fletcher (32-bits) and the second is the XOR Shift hash function.
-- 
-- Dependencies: Neorv32 source. 
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library neorv32;
use neorv32.neorv32_package.all;

entity hash_function_acceleration is
  port (
    -- global control --
    clk_i    : in  std_ulogic;                     -- global clock, rising edge
    rstn_i   : in  std_ulogic;                     -- global reset, low-active, async
    -- ctrl_i  : in  ctrl_bus_t;                   -- main control bus
    -- operation trigger --
    start_i  : in  std_ulogic;                     -- start trigger, single-shot
    -- operands --
    type_i   : in  std_ulogic;                     -- instruction type (0 = R-type, 1 = I-type)
    funct3_i : in  std_ulogic_vector(2 downto 0);  -- "funct3" bit-field from instruction word
    funct7_i : in  std_ulogic_vector(6 downto 0);  -- "funct7" bit-field from instruction word (R-type only)
    imm12_i  : in  std_ulogic_vector(11 downto 0); -- "imm12" bit-field from instruction word (I-type only)
    rs1_i    : in  std_ulogic_vector(31 downto 0); -- rf source 1 via "rs1" bit-field from instruction word
    rs2_i    : in  std_ulogic_vector(31 downto 0); -- rf source 2 via "rs2" bit-field from instruction word
    -- result and status --
    result_o : out std_ulogic_vector(31 downto 0); -- operation result
    valid_o  : out std_ulogic                      -- operation done
  );
end hash_function_acceleration;

architecture Behavioral of hash_function_acceleration is

    -- Indicates that the current instruction is R-type.
    constant r_type_c : std_ulogic := '0'; -- R-type CFU instructions (custom-0 opcode)
   
    -- Specify the funct3 bit-field value for the Fletcher Hash function.
    constant fletcher_32_func : std_ulogic_vector(2 downto 0) := "101";
    -- Specify the funct3 bit-field value for the XOR Shift Hash function.
    constant xor_shift_func   : std_ulogic_vector(2 downto 0) := "110";
   
    -- Define the numerical constants required for the two Hash functions.
    constant FLETCHER_CONST  : std_ulogic_vector(31 downto 0) := x"00010000";
    constant XOR_SHIFT_CONST : std_ulogic_vector(31 downto 0) := x"9E3779B1";

    -- Define the following record to keep track of the computational process. --
    type hash_t is record
        done : std_ulogic_vector(1 downto 0);
        opa  : std_ulogic_vector(31 downto 0); -- input operand a
        opb  : std_ulogic_vector(31 downto 0); -- input operand b
        res  : std_ulogic_vector(31 downto 0); -- operation results
    end record;
    
    signal hash : hash_t;

    -- The following signals are used as steps in Fletcher hash function. --
    signal fetcher_opa_and_ffff : std_ulogic_vector(31 downto 0);
    signal fetcher_opa_shift_16 : std_ulogic_vector(31 downto 0);
    signal fetcher_sum_of_opa   : std_ulogic_vector(31 downto 0);
    
    signal fetcher_opb_and_ffff       : std_ulogic_vector(31 downto 0);
    signal fetcher_opb_shift_16       : std_ulogic_vector(31 downto 0);
    signal fetcher_sum_of_opb_partial : std_ulogic_vector(31 downto 0);
    signal fetcher_sum_of_opb         : std_ulogic_vector(31 downto 0);

    signal fetcher_partial         : std_ulogic_vector(31 downto 0);
    signal fetcher_partial_shifted : std_ulogic_vector(31 downto 0);
    
    -- The following signals are used as steps in XOR Shift hash function --
    signal xor_sh_opb_shift_13 : std_ulogic_vector(31 downto 0);
    signal xor_sh_opb_shift_7  : std_ulogic_vector(31 downto 0);
    signal xor_sh_opb_xor      : std_ulogic_vector(31 downto 0);
    signal xor_sh_opa_xor      : std_ulogic_vector(31 downto 0);

begin

    hash_core: process(rstn_i, clk_i)
        -- xor_sh_mul_result variable contain the value of the multiplication 
        -- state of the xor shift hash function.
        variable xor_sh_mul_result: std_ulogic_vector(63 downto 0);
        begin
            if (rstn_i = '0') then
                hash.done <= (others => '0');
                hash.opa  <= (others => '0');
                hash.opb  <= (others => '0');
                hash.res  <= (others => '0');
            elsif rising_edge(clk_i) then
                hash.done(0) <= '0';
                hash.done(1) <= hash.done(0);

                -- Load the operands.
                if (start_i = '1') then
                    if (type_i = r_type_c) then
                        hash.done <= (others => '0');
                        hash.opa <= rs1_i;
                        hash.opb <= rs2_i;
                        hash.done(0) <= '1';
                    end if;
                end if;

                -- Calcualte the respective Hash function.
                if (hash.done(0) = '1') then
                    if (funct3_i(1 downto 0) = fletcher_32_func(1 downto 0)) then
                        -- Finish calculations for the Fletcher Hash function.
                        hash.res <= fetcher_partial_shifted or fetcher_sum_of_opa;
                    elsif (funct3_i(1 downto 0) = xor_shift_func(1 downto 0)) then
                        -- Finish calculations for the XOR Shift Hash function.
                        xor_sh_mul_result := std_ulogic_vector(unsigned(xor_sh_opa_xor) * unsigned(XOR_SHIFT_CONST));
                        -- Keep only the 32 lower bits.
                        hash.res <= xor_sh_mul_result(31 downto 0);
                    end if;
                end if;
            end if;
    end process hash_core;

    -- Start calculating the Fletcher Hash function.
    -- The following VHDL code executes the (A & 0xFFFF).
    fetcher_opa_and_ffff <= "0000000000000000" & hash.opa(15 downto 0);
    -- The following VHDL code executes the ((A >> 16) & 0xFFFF).
    fetcher_opa_shift_16 <= "0000000000000000" & hash.opa(31 downto 16);
    -- The following VHDL code executes the (s1 = (A & 0xFFFF) + ((A >> 16) & 0xFFFF)).
    fetcher_sum_of_opa   <= std_ulogic_vector(unsigned(fetcher_opa_and_ffff) + unsigned(fetcher_opa_shift_16));

    -- The following VHDL code executes the (B & 0xFFFF)
    fetcher_opb_and_ffff       <= "0000000000000000" & hash.opb(15 downto 0);
    -- The following VHDL code executes the ((B >> 16) & 0xFFFF).
    fetcher_opb_shift_16       <= "0000000000000000" & hash.opb(31 downto 16);
    -- The following VHDL code executes the (S1 + (B & 0xFFFF)).
    fetcher_sum_of_opb_partial <= std_ulogic_vector(unsigned(fetcher_sum_of_opa) + unsigned(fetcher_opb_and_ffff));
    -- The following VHDL code executes the (S1 + (B & 0xFFFF) + ((B >> 16) & 0xFFFF)).
    fetcher_sum_of_opb         <= std_ulogic_vector(unsigned(fetcher_sum_of_opb_partial) + unsigned(fetcher_opb_shift_16));

    -- The following VHDL code executes the (S2 = (S1 + (B & 0xFFFF) + ((B >> 16) & 0xFFFF)) % 65536).
    fetcher_partial <= std_ulogic_vector(unsigned(fetcher_sum_of_opb) mod unsigned(FLETCHER_CONST));
    -- The following VHDL code executes teh (S2 << 16). The last operation is done on the next clock.
    fetcher_partial_shifted <= fetcher_partial(15 downto 0) & "0000000000000000";

    -- Start calculating the XOR Shift Hash function.
    -- The following VHDL code executes the (B << 13).
    xor_sh_opb_shift_13 <= hash.opb(18 downto 0) & "0000000000000";
    -- The following VHDL code executes the (B >> 7).
    xor_sh_opb_shift_7  <= "0000000" & hash.opb(31 downto 7);
    -- The following VHDL code executes the ((B << 13) ^ (B >> 7)).
    xor_sh_opb_xor <= xor_sh_opb_shift_13 xor xor_sh_opb_shift_7;
    -- The following VHDL code executes the (A ^= (B << 13) ^ (B >> 7)).
    xor_sh_opa_xor <= hash.opa xor xor_sh_opb_xor;

    -- The following process selects the requested result. If the 
    -- opcode indicated that the function to be executed is the 
    -- XOR Shift Hash function, then it will output the XOR Shift result,
    -- the same goes for the Flitcher. 
    result_select: process(type_i, funct3_i, hash) 
    begin
        if (type_i = r_type_c) then
            case funct3_i is 
                when fletcher_32_func | xor_shift_func => 
                    -- Output the result of the coresponding Hash function.
                    result_o <= hash.res;
                    -- Output the DONE bit, to indicate when the operation
                    -- is done.
                    valid_o  <= hash.done(1);
                when others => 
                    result_o <= (others => '0');
                    valid_o <= '0';
            end case; 
        end if;
    end process result_select;

end Behavioral;
