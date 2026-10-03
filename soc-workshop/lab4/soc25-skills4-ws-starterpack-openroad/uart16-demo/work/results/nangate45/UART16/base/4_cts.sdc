###############################################################################
# Created by write_sdc
###############################################################################
current_design UART16
###############################################################################
# Timing Constraints
###############################################################################
create_clock -name core_clock -period 0.4600 [get_ports {clk}]
set_propagated_clock [get_clocks {core_clock}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {addr[0]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {addr[1]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {addr[2]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {baudclk16}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[0]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[10]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[11]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[12]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[13]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[14]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[15]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[1]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[2]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[3]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[4]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[5]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[6]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[7]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[8]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusin[9]}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {rwbar}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {sel}]
set_input_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {serin}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[0]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[10]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[11]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[12]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[13]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[14]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[15]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[1]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[2]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[3]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[4]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[5]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[6]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[7]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[8]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {dbusout[9]}]
set_output_delay 0.0920 -clock [get_clocks {core_clock}] -add_delay [get_ports {serout}]
###############################################################################
# Environment
###############################################################################
###############################################################################
# Design Rules
###############################################################################
