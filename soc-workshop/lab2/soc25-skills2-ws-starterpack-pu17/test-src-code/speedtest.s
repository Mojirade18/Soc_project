	;; A very simple speed test for PU17 implementation.
	;; All we really want are a bunch of cycles,
	;; it doesn't really matter what they do.

uart_data	.equ 0xf882
uart_status	.equ 0xf880
uart_txidle	.equ 1
uart_rxav	.equ 2
	
	.org	0x0

	lod	r1,#20		; outer count
outer:
;; 	lod     r2,#4000	; inner count
	lod     r2,#3		; inner count	
inner:
	sub     r2,#1
	cmp     r2,#0
	bne     inner

	mov	r3,#'.'
	str	r3,uart_data	; print a dot every time round inner loop
	
	sub	r1,#1
	cmp	r1,#0
	bne	outer
	

	mov	r2,#'O'		; print OK when done
	str	r2,uart_data
	mov	r2,#'K'
	str	r2,uart_data

	bra	0

	.end
	
