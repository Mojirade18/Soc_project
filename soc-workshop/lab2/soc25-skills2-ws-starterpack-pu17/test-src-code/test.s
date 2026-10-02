
	.text
	
	.org 0x4000

	str	r1,uart

	.bss
	.org 0x5000
uart:	.word 0

	



