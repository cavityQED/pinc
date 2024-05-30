set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 33 [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
set_property BITSTREAM.CONFIG.SPI_32BIT_ADDR NO [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 1 [current_design]
set_property BITSTREAM.CONFIG.SPI_FALL_EDGE YES [current_design]

create_clock -period 10.0 -name clk_0 -waveform {0.000 5.000} [get_ports clk]
set_property PACKAGE_PIN N14 [get_ports {clk}]
set_property IOSTANDARD LVCMOS33 [get_ports {clk}]

set_property PACKAGE_PIN T7 [get_ports {rst}];				set_property IOSTANDARD LVCMOS33 [get_ports {rst}];  

set_property PACKAGE_PIN C1 [get_ports {intr[0]}];				set_property IOSTANDARD LVCMOS33 [get_ports {intr[0]}];				set_property PULLUP true [get_ports {intr[0]}]
set_property PACKAGE_PIN C3 [get_ports {intr[1]}];				set_property IOSTANDARD LVCMOS33 [get_ports {intr[1]}];				set_property PULLUP true [get_ports {intr[1]}]
set_property PACKAGE_PIN E3 [get_ports {intr[2]}];				set_property IOSTANDARD LVCMOS33 [get_ports {intr[2]}];				set_property PULLUP true [get_ports {intr[2]}]
set_property PACKAGE_PIN D4 [get_ports {intr[3]}];				set_property IOSTANDARD LVCMOS33 [get_ports {intr[3]}];				set_property PULLUP true [get_ports {intr[3]}]

set_property PACKAGE_PIN M6 [get_ports {button_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[0]}];	set_property PULLUP true [get_ports {button_status[0]}]
set_property PACKAGE_PIN H5 [get_ports {button_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[1]}];	set_property PULLUP true [get_ports {button_status[1]}]
set_property PACKAGE_PIN J3 [get_ports {button_status[2]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[2]}];	set_property PULLUP true [get_ports {button_status[2]}]
set_property PACKAGE_PIN J5 [get_ports {button_status[3]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[3]}];	set_property PULLUP true [get_ports {button_status[3]}]
set_property PACKAGE_PIN K3 [get_ports {button_status[4]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[4]}];	set_property PULLUP true [get_ports {button_status[4]}]
set_property PACKAGE_PIN N9 [get_ports {button_status[5]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[5]}];	set_property PULLUP true [get_ports {button_status[5]}]
set_property PACKAGE_PIN R7 [get_ports {button_status[6]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[6]}];	set_property PULLUP true [get_ports {button_status[6]}]
set_property PACKAGE_PIN T9 [get_ports {button_status[7]}];		set_property IOSTANDARD LVCMOS33 [get_ports {button_status[7]}];	set_property PULLUP true [get_ports {button_status[7]}]


set_property PACKAGE_PIN T10 [get_ports {wheel_high}];			set_property IOSTANDARD LVCMOS33 [get_ports {wheel_high}];
set_property PACKAGE_PIN R6 [get_ports {wheel_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {wheel_status[0]}];	set_property PULLUP true [get_ports {wheel_status[0]}]
set_property PACKAGE_PIN P9 [get_ports {wheel_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {wheel_status[1]}];	set_property PULLUP true [get_ports {wheel_status[1]}]


set_property PACKAGE_PIN T8 [get_ports {x_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[0]}];		set_property PULLUP true [get_ports {x_status[0]}]
set_property PACKAGE_PIN T5 [get_ports {x_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[1]}];		set_property PULLUP true [get_ports {x_status[1]}]
set_property PACKAGE_PIN R8 [get_ports {x_status[2]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[2]}];		set_property PULLUP true [get_ports {x_status[2]}]
set_property PACKAGE_PIN L2 [get_ports {x_status[3]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[3]}];		set_property PULLUP true [get_ports {x_status[3]}]

set_property PACKAGE_PIN J1 [get_ports {y_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[0]}];		set_property PULLUP true [get_ports {y_status[0]}]
set_property PACKAGE_PIN H1 [get_ports {y_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[1]}];		set_property PULLUP true [get_ports {y_status[1]}]
set_property PACKAGE_PIN G1 [get_ports {y_status[2]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[2]}];		set_property PULLUP true [get_ports {y_status[2]}]
set_property PACKAGE_PIN K5 [get_ports {y_status[3]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[3]}];		set_property PULLUP true [get_ports {y_status[3]}]

set_property PACKAGE_PIN G5 [get_ports {spi_clk}];				set_property IOSTANDARD LVCMOS33 [get_ports {spi_clk}]
set_property PACKAGE_PIN F5 [get_ports {spi_miso}];				set_property IOSTANDARD LVCMOS33 [get_ports {spi_miso}]
set_property PACKAGE_PIN D6 [get_ports {spi_mosi}];				set_property IOSTANDARD LVCMOS33 [get_ports {spi_mosi}]
set_property PACKAGE_PIN C7 [get_ports {spi_cs}];				set_property IOSTANDARD LVCMOS33 [get_ports {spi_cs}];				set_property PULLUP true [get_ports {spi_cs}]