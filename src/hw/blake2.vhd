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

    type cfs_wr_regs_t is array (0 to 6) of std_ulogic_vector(31 downto 0);
    type cfs_rd_regs_t is array (0 to 3) of std_ulogic_vector(31 downto 0); 

    signal cfs_reg_wr : cfs_wr_regs_t; -- for WRITE accesses
    signal cfs_reg_rd : cfs_rd_regs_t; -- for READ accesses

    type blake_step_t is record 
        opa : STD_ULOGIC_VECTOR(31 downto 0);
        opb : STD_ULOGIC_VECTOR(31 downto 0);
        opc : STD_ULOGIC_VECTOR(31 downto 0);
        res : STD_ULOGIC_VECTOR(31 downto 0);
    end record; 
    
     signal phase : INTEGER;
        
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

    blakes_steps: process(clk_i)
    variable add_v_a_add_v_b_add_x : UNSIGNED(31 downto 0);
    variable add_v_c_add_v_d : UNSIGNED(31 downto 0);
    variable add_v_a_add_v_b_add_y: UNSIGNED(31 downto 0);
    variable v_d_rot_16 : STD_ULOGIC_VECTOR(31 downto 0);
    variable v_b_rot_12 : STD_ULOGIC_VECTOR(31 downto 0);
    variable v_d_rot_8: STD_ULOGIC_VECTOR(31 downto 0);
    begin
        if (rising_edge(clk_i)) then 
            if (cfs_reg_wr(CFS_ADDRESS_OF_STATUS) = x"0000000F") then 
                case phase is 
                    when 0 => 
                        v_a.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_A);
                        v_a.opb <= cfs_reg_wr(CFS_ADDRESS_OF_V_B);
                        v_a.opc <= cfs_reg_wr(CFS_ADDRESS_OF_X);
                        phase <= 1;
                    when 1 =>
                        add_v_a_add_v_b_add_x := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc); 
                        v_a.res <= STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_x);
                        v_d.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_D) xor STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_x);
                        phase <= 2;
                    when 2 =>
                        v_d_rot_16 := v_d.opa(15 downto 0) & v_d.opa(31 downto 16);
                        v_d.res <= v_d_rot_16;
                        v_c.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_C);
                        v_c.opb <= v_d_rot_16;
                        phase <= 3;
                    when 3 => 
                        add_v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);
                        v_c.res <= STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        v_b.opa <= cfs_reg_wr(CFS_ADDRESS_OF_V_B) xor STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        phase <= 4;
                    when 4 => 
                        v_b_rot_12 := v_b.opa(11 downto 0) & v_b.opa(31 downto 12);
                        v_b.res <= v_b_rot_12;
                        v_a.opa <= v_a.res;
                        v_a.opb <= v_b_rot_12;
                        v_a.opc <= cfs_reg_wr(CFS_ADDRESS_OF_Y);
                        phase <= 5;
                    when 5 =>
                        add_v_a_add_v_b_add_y := UNSIGNED(v_a.opa) + UNSIGNED(v_a.opb) + UNSIGNED(v_a.opc);
                        v_a.res <= STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_y);
                        v_d.opa <= v_d.res xor STD_ULOGIC_VECTOR(add_v_a_add_v_b_add_y);
                        phase <= 6;
                    when 6 =>
                        v_d_rot_8 := v_d.opa(7 downto 0) & v_d.opa(31 downto 8);
                        v_d.res <= v_d_rot_8;
                        v_c.opa <= v_c.res;
                        v_c.opb <= v_d_rot_8;
                        phase <= 7;
                    when 7 =>
                        add_v_c_add_v_d := UNSIGNED(v_c.opa) + UNSIGNED(v_c.opb);
                        v_c.res <= STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        v_b.opa <= v_b.res xor STD_ULOGIC_VECTOR(add_v_c_add_v_d);
                        phase <= 8;
                    when 8 =>
                        v_b.res <= v_b.opa(6 downto 0) & v_b.opa(31 downto 7);
                        irq_o <= '1';
                        phase <= 9;
                    when 9 => 
                        if (cfs_reg_wr(CFS_ADDRESS_OF_STATUS) = x"00000000") then 
                            phase <= 0;
                            irq_o <= '0';
                        end if;
                    when others => phase <= 0; 
                end case; 
            end if;

        end if;
    end process blakes_steps;


cfs_reg_rd(CFS_ADDRESS_OF_V_A) <= v_a.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_B) <= v_b.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_C) <= v_c.res;
cfs_reg_rd(CFS_ADDRESS_OF_V_D) <= v_d.res;

end neorv32_cfs_rtl;
