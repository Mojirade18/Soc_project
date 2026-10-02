#
# SoC-25 - SoC Lab Videos
#
# Module 3: SoC Design Methodologies
# SoC Design Methodologies using Fast Models and C++/SystemC Modelling for Architectural Exploration. 
# (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.
#
#
# You will also need the software starter pack from Skills Workshop 1.

# Location of where gem5 has been installed in src form.
GEM5SRC ?=/home/djg11/d510/gem5/gem5/src


# Make targets include verilator_simulate c_model mk_starterpack # build2

all: mk_starterpack  verilator_simulate





spare:
	verilator -cc soc25-tpu.v
#	cd obj_dir; make -f Vsoc25__02dtpu.mk
	g++ -I/usr/share/verilator/include -Iobj_dir cbg_verilator_main.cpp obj_dir/Vsoc25__02dtpu.cpp \
	/usr/share/verilator/include/verilated.cpp \
	obj_dir/Vsoc25__02dtpu__Syms.cpp

#---------------------------------------------------------
# Verilate and then simulate.
# Make sure you install verilator-devel if using Fedora
verilator_simulate: soc25-ttpu.v soc25_ttpu_verilator_main.cpp
	verilator -cc soc25-ttpu.v
	cd obj_dir;  VERILATOR_ROOT=/usr/share/verilator make -f Vsoc25__02dttpu.mk
	g++ -DVL_TIME_STAMP64 -I/usr/share/verilator/include -Iobj_dir soc25_ttpu_verilator_main.cpp  obj_dir/verilated.o obj_dir/Vsoc25__02dttpu__ALL.a \
	obj_dir/verilated_threads.o
	./a.out


#-----------------------------------------------------------------
# Standlone C compilation, just to check syntax
c_model:
	gcc -c soc25-ttpu.c

#-----------------------------------------------------------------
# Yosis synthesis
synth:
	yosys  -s yosys.tcl

#-----------------------------------------------------------------
# Create a starter pack
SPD=soc25-skills3a-ws-starterpack-gem5-fastmodel
SPNAME=$(SPD).zip
SSP=small-slide-pack/soc25-skills3-intro.pdf

mk_starterpack:sp_kill $(SPNAME)
sp_kill:
	rm -fr $(SPNAME) $(SPD)

I1FOLDER=../outgoing/soc25-one-a-gem5/images
I3FOLDER=../outgoing/soc25-three-a-gem5/images
PICTURES1= \
  $(I3FOLDER)/generic-soc-202.png \
  $(I1FOLDER)/Arm-KMI-block-diagram.png \
  $(I1FOLDER)/ArmPrimeCell-PL011-block-diagram.png \
  $(I1FOLDER)/gem5-setup-one.png \
  $(I1FOLDER)/IMG_20250401_173646-KM-interface.jpg \
  $(I1FOLDER)/IMG_20250401_180106-PS2-and-serial-and-USB.jpg \
  $(I1FOLDER)/soc25-gem5-mymon-experiment-sw-struct.png \
  $(I1FOLDER)/ve-config.dot.png \
  $(I1FOLDER)/VersatileExpressPCBoard.jpg


PICTURES3= \
  $(I3FOLDER)/soc25-esl-gem5.png \
  $(I3FOLDER)/soc25-ttpu.png \
  $(I3FOLDER)/vtoc-ctov.png \
  $(I3FOLDER)/withos.png



$(SPNAME):
	rm -rf $(SPD)
	mkdir $(SPD)
	rm -r pictures ; mkdir pictures ; cp $(PICTURES1) $(PICTURES3) pictures
	cp -r pictures README yosys.tcl /home/djg11/Dropbox/d-d787/Arm-Haremi/Three-SoC-Lab-Exercises-or-Skills-Videos/copyright_message.txt $(SPD)
	rm -r pictures
	mkdir -p $(SPD)/misc
	cp  Makefile soc25_ttpu_verilator_main.cpp soc25-ttpu.v $(SSP) example-spool $(SPD)
	cp $(GEM5SRC)/dev/misc/SConscript  $(GEM5SRC)/dev/misc/*hh $(GEM5SRC)/dev/misc/*cc $(GEM5SRC)/dev/misc/*py $(SPD)/misc
	mkdir -p $(SPD)/arm
	cp  $(GEM5SRC)/dev/arm/SConscript $(GEM5SRC)/dev/arm/ttpu101.hh $(GEM5SRC)/dev/arm/ttpu101.cc $(GEM5SRC)/dev/arm/Ttpu101.py    $(SPD)/arm
	cp $(GEM5SRC)/dev/arm/soc25-ttpu.c $(GEM5SRC)/dev/arm/soc25-ttpu.h    $(SPD)/arm
	cp $(GEM5SRC)/dev/arm/RealView.py  $(SPD)
	zip -r $(SPNAME) $(SPD)
	ls -l $(SPNAME)
	unzip -l $(SPNAME)

# eof
