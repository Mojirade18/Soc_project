
	;;
  	;; PU17 bootrom


uart_data	.equ 0x83	// Low byte at plus one.
uart_status	.equ 0x80
uart_txidle	.equ 1
uart_rxav	.equ 2

startpoint	.equ	0x2000

	.org	0x0


	
	mov R0,#1 ; lt
	 and R1,R0 ; XRP
	 beq br167
	 bra br159
	
br167:	
	
br159
;   }


	
	bra br167
	
	mov r0,#3
	mov r1,#5
	mov r2,#7
	mov r3,#9
	mov r7,#32
	stm { r0, r1, r2-r3, r7 } 
	
	lod	r2,lit_startpoint

 	;;  Memory dump
ps0
	lod	r0,uart_status
	and	r0,#uart_txidle 		; Wait for read to send fjmp
	beq	ps0 ; fj
	lodb	r0,[r2]
	str	r0,uart_data
	add	r2,#1
	cmp	r2,#lit_startpoint_128
	
	bne	ps0

	lod	r2,#lit_startpoint	
ps1:	lod	r0,uart_status
	and	r0,#uart_rxav
	beq	ps1		; Wait for char to arrive
	lodb	r0,uart_data
	strb	r0,[r2]
	add	r0,#1
	strb	r0,uart_data
	add	r2,#1
	cmp	r0,#0xEE
	bne	ps1
	jmp	startpoint

lit_startpoint:		.word startpoint
lit_startpoint_128:	.word startpoint+128
	
	
	.end
	