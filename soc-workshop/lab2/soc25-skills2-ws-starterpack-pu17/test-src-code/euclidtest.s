	;; A demo of Euclid's Algorithm on the PU17 processor.
	;; (C) DJ Greaves, 1998-2024 University of Cambridge, Computer Laboratory.
	;; 
	;;  Wikipedia: 	The Euclidean algorithm is based on the principle that the greatest common divisor of two
	;;  numbers does not change if the larger number is replaced by its difference with the smaller number.

;;	Set up the addresses for programmed I/O access to the UART
uart_data	.equ 0xf882
uart_status	.equ 0xf880
uart_txidle	.equ 1 		; Status register bit: ready for new tx data
uart_rxav	.equ 2		; Status register bit: fresh rx data is available.
	
	.org	0x0		; Origin - load at zero, the PC reset vector value.

	lod	R1,#50		; First argument for Euclid
	lod	R2,#525		; Second argument	

euclid_loop:		 ; Swap R1 and R2 so that R2 is smaller
	cmp	R2,R1
	ble	dont_swap
	mov	R3,R1
	mov	R1,R2
	mov 	R2,R3
dont_swap:		 ; Now see if values are equal.
	cmp	R2,R1
	beq	finished ; If so, we have finished.
	sub	R1,R2	 ; Subtract smaller from the larger
	bra	euclid_loop

finished:			; One of the values is unity
	;; So the HCF is in either of the registers, R1 or R2.
	;; If that is also unity, they were coprime.

	;; Later we'll see a proper decimal print routine,
	;; but here we just hope the answer is less than 8 bits and write it to the UART.	

	str	R1,uart_data

	
	mov	r2,#'O'		; print OK when done
	str	r2,uart_data
	mov	r2,#'K'
	str	r2,uart_data

self:	bra	self		; We don't have a HALT instruction, so hang

	.end
	
