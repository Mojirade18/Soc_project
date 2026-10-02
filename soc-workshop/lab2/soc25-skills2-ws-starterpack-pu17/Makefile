#
# (C) DJ Greaves, 1998-2024 University of Cambridge, Computer Laboratory.
#
# SoC-25 Makefile to simulate PU17 processor using Icarus Verilog (will also work with any other Verilog simulator).
#

all: clean iverilog_euclid_test

LOSIMEXFNAME    ?=/tmp/losim_iverilog.ivf
PU17_SYSTEM_RTL =rtl/pu17_testbench.v rtl/pu17.v rtl/xcadders.v rtl/uart16.v rtl/CBG_SSRAM0_16K_X_16.v

# Icarus Verilog simulation 1
iverilog_euclid_test: EUCLIDTEST_ROM.v
	iverilog -o $(LOSIMEXFNAME) -s SIMSYS $(PU17_SYSTEM_RTL) EUCLIDTEST_ROM.v
	$(LOSIMEXFNAME)


EUCLIDTEST_ROM.v:EUCLIDTEST_ROM.ihex tools/xromgen
	./tools/xromgen -v EUCLIDTEST_ROM.v -n CANNED_PUTEST_ROM 16 EUCLIDTEST_ROM.ihex


EUCLIDTEST_ROM.ihex:test-src-code/euclidtest.s tools/aspu17
	./tools/aspu17 -verbose test-src-code/euclidtest.s EUCLIDTEST_ROM.ihex listing symbols


# Compile the tool that creates an RTL from Intel hex file.
xromgen:tools/xromgen.c
	gcc -o xromgen tools/xromgen.c

# Compile the assembler
aspu17:tools/aspu17.c
	gcc -o aspu17 tools/aspu17.c



clean:
	rm -f tools/aspu17 tools/xromgen listing tools/downtobin 


# Spare target, not needed in this workshop: but note that mymon supports download
# of further programs into RAM using intel hex, as generated here.
download:aspu17 download.s 
	aspu17 download.s download listing symbols
	downtobin download


#---------------------------------------------------------
# Create a starter pack




I2FOLDER=../outgoing/soc25-two
PICTURES2= \
  $(I2FOLDER)/gtkwave-pu17-screenshot.png \
  $(I2FOLDER)/pu17.png \
  $(I2FOLDER)/pu17-testbench-diagram.png



START_PACK_NAME=soc25-skills2-ws-starterpack-pu17.zip
START_PACK_BUILD=soc25-skills2-ws-starterpack-pu17
START_PACK_UNBUILD=/tmp/soc25-skills2-ws-starterpack-pu17-unbuild

mk_starterpack: ../copyright_message.txt  $(PU17_SYSTEM_RTL) README tools/*.c
	rm -rf  /tmp/$(START_PACK_NAME) /tmp/$(START_PACK_BUILD)
	mkdir -p /tmp/$(START_PACK_BUILD)/tools /tmp/$(START_PACK_BUILD)/rtl /tmp/$(START_PACK_BUILD)/pictures
	rm -f pictures ; mkdir pictures ; cp $(PICTURES2)  /tmp/$(START_PACK_BUILD)/pictures
	cp -r Makefile README example-spool.txt ../copyright_message.txt test-src-code /tmp/$(START_PACK_BUILD)
	cp tools/*.c  /tmp/$(START_PACK_BUILD)/tools
	cp $(PU17_SYSTEM_RTL) /tmp/$(START_PACK_BUILD)/rtl
	cd /tmp ; zip -r $(START_PACK_NAME)  $(START_PACK_BUILD)
	mv /tmp/$(START_PACK_NAME) .
	unzip -l  $(START_PACK_NAME)  


test_starterpack: mk_starterpack
	rm -rf $(START_PACK_UNBUILD)
	mkdir $(START_PACK_UNBUILD)
	cp $(START_PACK_NAME) $(START_PACK_UNBUILD)
	cd $(START_PACK_UNBUILD) ; unzip $(START_PACK_NAME)
	cd $(START_PACK_UNBUILD)/$(START_PACK_BUILD) ; ls -l ; make
	echo "Starter pack test complete"

# eof



