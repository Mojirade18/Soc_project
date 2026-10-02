#
# SoC-25 Workshop III synthesis script.
# yosys.tcl   
#
read_verilog soc25-ttpu.v

# generic synthesis
synth -top SOC25_TTPU101

# mapping to mycells.lib
dfflibmap -liberty NangateOpenCellLibrary_typical.lib
abc -liberty NangateOpenCellLibrary_typical.lib
clean

# write synthesized design
write_verilog synth.v

# eof
tee -o area.txt stat -liberty NangateOpenCellLibrary_typical.lib
