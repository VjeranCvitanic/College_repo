library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

use ieee.numeric_std.all;
use std.textio.all;

entity movavg4 is
  port (
	clk : in std_logic;
	rst : in std_logic;

	-- Adresna sabirnica
	port_id : in std_logic_vector(7 downto 0);

	-- Podatkovna sabirnica za pisanje u registar, spaja se na out_port od KCPSM6
	in_port : in std_logic_vector(7 downto 0);
	write_strobe : in std_logic;

	-- Podatkovna sabirnica za čitnaje iz registra, spaja se na in_port od KCPSM6
	out_port : out std_logic_vector(7 downto 0);
	read_strobe : in std_logic

  ) ;
end entity ; -- movavg4

architecture arch of movavg4 is -- Nadopunite opis arhitekture sklopa
	signal D1: std_logic_vector (7 downto 0) := "00000000";
	signal D2: std_logic_vector (7 downto 0) := "00000000";
	signal D3: std_logic_vector (7 downto 0) := "00000000";
	signal D4: std_logic_vector (7 downto 0) := "00000000";

	signal RES: std_logic_vector(9 downto 0) := "0000000000";

	signal counter: std_logic_vector (1 downto 0) := "00";
	signal start:std_logic := '0';

begin
	process(clk, rst)
	begin
		if(rst = '1') then
			D1 <= "00000000";
			D2 <= "00000000";
			D3 <= "00000000";
			D4 <= "00000000";
			RES <= "0000000000";
			start <= '0';
			counter <= "00";
		end if;
		
		if rising_edge(clk) then
			if write_strobe = '1' then
				if port_id = "00000010" then
					D1 <= "00000000";
					D2 <= "00000000";
					D3 <= "00000000";
					D4 <= "00000000";
					RES <= "0000000000";
					start <= '0';
					counter <= "00";
				elsif port_id = "00000000" then
					if(start = '0') then
						if(counter = "10") then
							start <= '1';
						end if;
						counter <= counter + '1';
					end if;
					if(start = '1') then
						RES <= ("00" & D1) + ("00" & D2) + ("00" & D3) + ("00" & in_port);
					end if;
					D4 <= D3;
					D3 <= D2; 
					D2 <= D1;
					D1 <= in_port;
				end if;
			elsif(read_strobe = '1' and port_id = "01") then
				out_port <= RES (9 downto 2);
			end if;
		end if;
	end process;

end architecture arch;
