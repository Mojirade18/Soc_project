#
# Makefile for SoC-25 GEM5 Skills Video. (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.
#

# mymon running bare metal on an Arm 9 - for emulation or real board use.

# Where installed
GEM5HOME=$(HOME)/gem5
PROJECT_FOLDER=/home/djg11/Dropbox/d-d787/Arm-Haremi/Three-SoC-Lab-Exercises-or-Skills-Videos
FIRST_FS= $(GEM5HOME)/configs/deprecated/example/fs.py


all: gem5_run


# Locally built (and extended in workshop 3) gem5 system.
gem5_run: built.elf
	time $(GEM5HOME)/build/ARM/gem5.opt  \
	    $(FIRST_FS) \
	    --bare-metal \
	    --kernel=$(CURDIR)/built.elf \
	    --machine-type=VExpress_GEM5_V1



givehelp:
	$(GEM5HOME)/build/ARM/gem5.opt \
	    $(GEM5HOME)/configs/deprecated/example/fs.py --give-cpu-types --help


# AOPS = --warn --fatal-warnings -mcpu=cortex-m0
ARMGNU ?= arm-linux-gnueabihf
TARCH = -march=armv9-a -marm -mfloat-abi=soft -fno-pie
ASFLAGS = --warn --fatal-warnings 
CCFLAGS = -g -Wall -O0 -nostdlib -nostartfiles $(TARCH)

built.elf:  cbg_baremetal_crt.o kickoff_baremetal.o cbg_uart_driver.o mylinein.o prlibc.o mymon.o  memmap
	$(ARMGNU)-ld cbg_baremetal_crt.o  mylinein.o prlibc.o mymon.o cbg_uart_driver.o kickoff_baremetal.o -T memmap -o built.elf
	$(ARMGNU)-objdump -d built.elf > built.dis

buildimage.bin:builf.elf
	$(ARMGNU)-objcopy built.elf -O binary buildimage.bin

cbg_baremetal_crt.o: cbg_baremetal_crt.s
	$(ARMGNU)-as $(ASFLAGS) cbg_baremetal_crt.s -o cbg_baremetal_crt.o

kickoff_baremetal.o: kickoff_baremetal.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) kickoff_baremetal.c

mylinein.o: mylinein.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) mylinein.c

prlibc.o: prlibc.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) -DHEAPBASE=0x20000 -DHEAPEND=0x30000 prlibc.c

mymon.o: mymon.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) mymon.c

cbg_uart_driver.o: cbg_uart_driver.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) cbg_uart_driver.c
	arm-linux-gnueabihf-objdump -d cbg_uart_driver.o

armdis.o: armdis.c 
	$(ARMGNU)-gcc -c $(CCFLAGS) armdis.c

clean:
	rm -f *.o built.elf built.dis

#------------------------------------------------------------
# Starter pack build
I1FOLDER=../../outgoing/soc25-one-a-gem5/images
I3FOLDER=../../outgoing/soc25-three-a-gem5/images
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


START_PACK_NAME=soc25-skills1a-ws-starterpack-gem5.zip
mk_starterpack: cbg_baremetal_crt.s \
	     cbg_uart_driver.c \
	     cbg_zynq_uart_driver.c \
	     kickoff_baremetal.c \
	     Makefile \
	     mylinein.c \
	     mymon.c memmap \
	     prstdio.h prlibc.c 
	cp ../../copyright_message.txt .
	rm -f  $(START_PACK_NAME)
	rm -f pictures ; mkdir pictures ; cp $(PICTURES1) pictures
	zip -r $(START_PACK_NAME)  \
	     copyright_message.txt \
	     pictures \
	     cbg_baremetal_crt.s \
	     cbg_uart_driver.c \
	     cbg_zynq_uart_driver.c \
	     kickoff_baremetal.c \
	     Makefile \
	     mylinein.c \
	     mymon.c memmap \
	     prstdio.h prlibc.c \
	     typical-spool.txt
	unzip -l  $(START_PACK_NAME)  
	rm -r pictures

test_starterpack: mk_starterpack
	rm -rf /tmp/sptest
	mkdir /tmp/sptest
	cp $(START_PACK_NAME) /tmp/sptest
	cd /tmp/sptest ; unzip $(START_PACK_NAME); make

# eof

