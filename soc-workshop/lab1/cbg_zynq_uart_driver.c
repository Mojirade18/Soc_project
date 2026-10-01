//---------------------------------
// cbg_uart_driver.c
// arm virtex7 zynq
// SoC-25 QEMU Skills Video. (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.


//-------------------------------------------------------------------


#define UART_BASE0 0xe0000000 // Just using UART0 for now
#define UART_BASE1 0xe0001000
#define UART0_CR      (UART_BASE0 + 0x0)
#define UART0_MR      (UART_BASE0 + 0x4)
#define UART0_BAUDGEN (UART_BASE0 + 0x18)
#define UART0_SR      (UART_BASE0 + 0x2C)
#define UART0_DR      (UART_BASE0 + 0x30)
#define UART0_BAUDDIV (UART_BASE0 + 0x34)

#define IOREAD32(ADDR)          (*((unsigned volatile int *)ADDR))
#define IOWRITE32(ADDR, WDATA)  { *((unsigned int *)ADDR) = WDATA; }
//-------------------------------------------------------------------
void cbg_uart_wrch(char cc)
{
  while (IOREAD32(UART0_SR)&(1<<14)) continue; // Spin on nearly full bit (FS)
  IOWRITE32(UART0_DR, cc);
}
//-------------------------------------------------------------------
char cbg_uart_rdch (void)
{
  while (IOREAD32(UART0_SR)&(1<<1)) continue; // Spin while RES bit set (receive empty status)
  return (IOREAD32(UART0_DR)) & 127;
}
//-------------------------------------------------------------------
void cbg_uart_init(void)
{
  IOWRITE32(UART0_CR, 3); // Set reset bits.
  while (IOREAD32(UART0_CR) & 3) continue; // Spin until bits gond.
  // Note we expect a reset valu of 0x28b in BAUDGEN_OFFSET register at offset 0x18
  IOWRITE32(UART0_BAUDGEN, 0x7c); // Select 115200 baud
  IOWRITE32(UART0_BAUDDIV, 6);
  IOWRITE32(UART0_MR, 0x20); // No parity, 8 bits, one stop bit.
  IOWRITE32(UART0_CR, (1<<2) | (1<<4)); // Set bits 2+4 for TX+RX enable,  
  IOREAD32(UART0_DR);
}

// Low-level title banner write.
void cbg_uart_puts(const char *msg)
{
  while (*msg)
    {
      char c = *msg++;
      if (c == 0x0A)   cbg_uart_wrch(0x0D);
      cbg_uart_wrch(c);
    }
}


int g_console_flags = 0; // This will be set on control-C or break in the interrup-driven variant.

//
// Calling this function makes the SystemC or QEMU simulation exit.
//
void emulator_exit_via_backdoor(char *msg, int return_value)
{
#ifdef OR1K  
  asm("l.addi\tr3,%0,0": :"r" (value));
  asm("l.nop %0": :"K" (NOP_EXIT)); // Exit backdoor instruction.
#else
  register unsigned int r0 __asm__("r0");
  r0 = 0x18;
  register unsigned r1 __asm__("r1");
  r1 = 0x20026;
  __asm__ volatile("bkpt #0xAB");
#endif  
}


void spin_delay() // aka core_pause()
{
  __asm__("yield");
  __asm__("yield");
  __asm__("yield");

}
// eof
