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

set_property PACKAGE_PIN M6 [get_ports {rst}];				set_property IOSTANDARD LVCMOS33 [get_ports {rst}];            set_property PULLTYPE PULLUP [get_ports {rst}]

set_property PACKAGE_PIN R13 [get_ports {out[0]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[0]}]
set_property PACKAGE_PIN R12 [get_ports {out[1]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[1]}]
set_property PACKAGE_PIN R10 [get_ports {out[2]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[2]}]
set_property PACKAGE_PIN N3 [get_ports {out[3]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[3]}]
set_property PACKAGE_PIN P4 [get_ports {out[4]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[4]}]
set_property PACKAGE_PIN L4 [get_ports {out[5]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[5]}]
set_property PACKAGE_PIN M5 [get_ports {out[6]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[6]}]
set_property PACKAGE_PIN P5 [get_ports {out[7]}];			set_property IOSTANDARD LVCMOS33 [get_ports {out[7]}]

set_property PACKAGE_PIN C1 [get_ports {intr[0]}];			set_property IOSTANDARD LVCMOS33 [get_ports {intr[0]}];        set_property PULLTYPE PULLUP [get_ports {intr[0]}]
set_property PACKAGE_PIN C3 [get_ports {intr[1]}];			set_property IOSTANDARD LVCMOS33 [get_ports {intr[1]}];        set_property PULLTYPE PULLUP [get_ports {intr[1]}]
set_property PACKAGE_PIN E3 [get_ports {intr[2]}];			set_property IOSTANDARD LVCMOS33 [get_ports {intr[2]}];        set_property PULLTYPE PULLUP [get_ports {intr[2]}]

set_property PACKAGE_PIN A7 [get_ports {x_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[0]}];     set_property PULLTYPE PULLUP [get_ports {x_status[0]}]
set_property PACKAGE_PIN B5 [get_ports {x_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[1]}];     set_property PULLTYPE PULLUP [get_ports {x_status[1]}]
set_property PACKAGE_PIN A4 [get_ports {x_status[2]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[2]}];		set_property PULLTYPE PULLUP [get_ports {x_status[2]}]
set_property PACKAGE_PIN A3 [get_ports {x_status[3]}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_status[3]}];		set_property PULLTYPE PULLUP [get_ports {x_status[3]}]

set_property PACKAGE_PIN F3 [get_ports {x_alarm}];			set_property IOSTANDARD LVCMOS33 [get_ports {x_alarm}]
set_property PACKAGE_PIN D1 [get_ports {x_en_in}];			set_property IOSTANDARD LVCMOS33 [get_ports {x_en_in}]
set_property PACKAGE_PIN A2 [get_ports {x_dir_in}];			set_property IOSTANDARD LVCMOS33 [get_ports {x_dir_in}]
set_property PACKAGE_PIN E1 [get_ports {x_step_in}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_step_in}]
set_property PACKAGE_PIN T8 [get_ports {x_step_out}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_step_out}]
set_property PACKAGE_PIN T5 [get_ports {x_dir_out}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_dir_out}]
set_property PACKAGE_PIN R8 [get_ports {x_en_out}];			set_property IOSTANDARD LVCMOS33 [get_ports {x_en_out}]

set_property PACKAGE_PIN T13 [get_ports {x_limit}];			set_property IOSTANDARD LVCMOS33 [get_ports {x_limit}]; 	set_property PULLTYPE PULLUP [get_ports {x_limit}] 
set_property PACKAGE_PIN T12 [get_ports {x_limit_com}];		set_property IOSTANDARD LVCMOS33 [get_ports {x_limit_com}];


set_property PACKAGE_PIN B7 [get_ports {y_status[0]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[0]}];		set_property PULLTYPE PULLUP [get_ports {y_status[0]}]
set_property PACKAGE_PIN B6 [get_ports {y_status[1]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[1]}];		set_property PULLTYPE PULLUP [get_ports {y_status[1]}]
set_property PACKAGE_PIN A5 [get_ports {y_status[2]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[2]}];		set_property PULLTYPE PULLUP [get_ports {y_status[2]}]
set_property PACKAGE_PIN B4 [get_ports {y_status[3]}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_status[3]}];		set_property PULLTYPE PULLUP [get_ports {y_status[3]}]

set_property PACKAGE_PIN F4 [get_ports {y_alarm}];			set_property IOSTANDARD LVCMOS33 [get_ports {y_alarm}]
set_property PACKAGE_PIN E2 [get_ports {y_en_in}];			set_property IOSTANDARD LVCMOS33 [get_ports {y_en_in}]
set_property PACKAGE_PIN B2 [get_ports {y_dir_in}];			set_property IOSTANDARD LVCMOS33 [get_ports {y_dir_in}]
set_property PACKAGE_PIN F2 [get_ports {y_step_in}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_step_in}]
set_property PACKAGE_PIN T7 [get_ports {y_step_out}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_step_out}]
set_property PACKAGE_PIN R5 [get_ports {y_dir_out}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_dir_out}]
set_property PACKAGE_PIN P8 [get_ports {y_en_out}];			set_property IOSTANDARD LVCMOS33 [get_ports {y_en_out}]

set_property PACKAGE_PIN R11 [get_ports {y_limit}];			set_property IOSTANDARD LVCMOS33 [get_ports {y_limit}]; 	set_property PULLTYPE PULLUP [get_ports {y_limit}] 
set_property PACKAGE_PIN N2 [get_ports {y_limit_com}];		set_property IOSTANDARD LVCMOS33 [get_ports {y_limit_com}];

set_property PACKAGE_PIN K5 [get_ports {wheel_a}];			set_property IOSTANDARD LVCMOS33 [get_ports {wheel_a}]
set_property PACKAGE_PIN E6 [get_ports {wheel_b}];			set_property IOSTANDARD LVCMOS33 [get_ports {wheel_b}]

set_property PACKAGE_PIN G5 [get_ports {spi_clk}];			set_property IOSTANDARD LVCMOS33 [get_ports {spi_clk}] 
set_property PACKAGE_PIN F5 [get_ports {spi_miso}];			set_property IOSTANDARD LVCMOS33 [get_ports {spi_miso}]
set_property PACKAGE_PIN D6 [get_ports {spi_mosi}];			set_property IOSTANDARD LVCMOS33 [get_ports {spi_mosi}]
set_property PACKAGE_PIN C7 [get_ports {spi_cs}];			set_property IOSTANDARD LVCMOS33 [get_ports {spi_cs}]
