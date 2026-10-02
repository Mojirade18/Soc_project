	;; A hello world program.

uart_data	.equ 0xf882
uart_status	.equ 0xf880
uart_txidle	.equ 1
uart_rxav	.equ 2
	

mon	.equ 0x40

	.org	0x0

	lod	r3,#0x8100	;  Set a stack pointer
	lod 	r1,#'H'


lab1:
	
	str	r1,uart_data

	
	lod	r0,#hwtext
		
print_string:
	mov	r1,r0
	
ps1:	lod	r0,uart_status
	and	r0,#uart_txidle
	beq	ps1		; Wait for idle flag set
	lodb	r0,[r1]
	cmp	r0,#0
	beq	ps999
	str	r0,uart_data
	add	r1,#1
	bra	ps1

ps999:	bra     0

hwtext:		.defb	"ello World, OK",0

	;; 'OK' causes the test to pass

	.end
	
