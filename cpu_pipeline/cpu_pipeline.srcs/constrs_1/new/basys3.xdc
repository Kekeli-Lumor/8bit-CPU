## Clock signal (100MHz onboard oscillator)
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk]

## Reset - mapped to a pushbutton (BTNC, center button)
set_property PACKAGE_PIN U18 [get_ports reset]
set_property IOSTANDARD LVCMOS33 [get_ports reset]

## Debug outputs - mapped to onboard LEDs (8 of the 16 available)
set_property PACKAGE_PIN U16 [get_ports {debug_wb_data[0]}]
set_property PACKAGE_PIN E19 [get_ports {debug_wb_data[1]}]
set_property PACKAGE_PIN U19 [get_ports {debug_wb_data[2]}]
set_property PACKAGE_PIN V19 [get_ports {debug_wb_data[3]}]
set_property PACKAGE_PIN W18 [get_ports {debug_wb_data[4]}]
set_property PACKAGE_PIN U15 [get_ports {debug_wb_data[5]}]
set_property PACKAGE_PIN U14 [get_ports {debug_wb_data[6]}]
set_property PACKAGE_PIN V14 [get_ports {debug_wb_data[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {debug_wb_data[*]}]

set_property PACKAGE_PIN V13 [get_ports debug_halted]
set_property IOSTANDARD LVCMOS33 [get_ports debug_halted]

## Debug: write-back address (3 bits) and write-enable - mapped to 4 more LEDs
set_property PACKAGE_PIN V3  [get_ports {debug_wb_addr[0]}]
set_property PACKAGE_PIN W3  [get_ports {debug_wb_addr[1]}]
set_property PACKAGE_PIN U3  [get_ports {debug_wb_addr[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {debug_wb_addr[*]}]

set_property PACKAGE_PIN P3  [get_ports debug_wb_enable]
set_property IOSTANDARD LVCMOS33 [get_ports debug_wb_enable]