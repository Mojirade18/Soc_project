

uart_data	.equ 0x83
uart_status	.equ 0x80
uart_txidle	.equ 1
uart_rxav	.equ 2
	

	.org	0x2000

	

	lod	r3,#0xFE	;  Set a stack pointer
	


ps0:	lod	r1,uart_status
	and	r1,#uart_rxav
	beq	ps0
	lodb	r0,uart_data		
	
	
ps1:	lod	r1,uart_status
	and	r1,#uart_txidle
	beq	ps1		; Wait for idle flag set
	strb	r0,uart_data
	add	r0,#1
	cmp	r0,#0x3A
	
	bne	ps1

ps2:	lod	r1,uart_status
	and	r1,#uart_txidle
	beq	ps2		; Wait for idle flag set
	mov	r0,#'Y'
	strb	r0,uart_data
	
	jmp  	ps0
	
	

	.end
	