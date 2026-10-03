#
# config-soc25-uart-openroad-demo.mk
#
export DESIGN_NAME = UART16
export PLATFORM    = nangate45

export VERILOG_FILES = $(DESIGN_HOME)/uart16.v
export SDC_FILE      = $(DESIGN_HOME)/constraint-soc25-uart-openroad-demo.sdc
export ABC_AREA      = 1

export CORE_UTILIZATION ?= 40
export PLACE_DENSITY_LB_ADDON = 0.20
export TNS_END_PERCENT        = 100
export REMOVE_CELLS_FOR_EQY   = TAPCELL*

# eof
