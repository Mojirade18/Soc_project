	# SoC-25 QEMU Skills Video. (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.
	# cbg_baremetal_crt.s
	# This code sits at the bottom of the address space and provides stubs for the various
	# interrupt and reset vectors.

.globl _start
_start:
    b hw_reset
    b spin_forever
    b spin_forever
    b spin_forever

    b spin_forever
    b spin_forever
    b spin_forever
    b spin_forever

    b spin_forever
    b spin_forever
    b spin_forever
    b spin_forever

    b spin_forever
    b spin_forever
    b spin_forever
    b spin_forever

hw_reset:
    mov sp,#0x10000
    bl notmain

	
spin_forever:
	# What is the QEMU backdoor to cause 
	b spin_forever


#eof
