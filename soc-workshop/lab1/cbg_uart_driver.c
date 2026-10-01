//---------------------------------
// cbg_uart_driver.c

// Arm Primecell DDI0183G_uart_pl011_r1p5_trm.pdf (mostly the same as a 16C650)

// SoC-25 QEMU Skills Video. (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.


//-------------------------------------------------------------------


#define UART_BASE0 0x1c090000  // Just using UART0 for now
#define UART0_DR      (UART_BASE0 + 0x00) // Data register
#define UART0_SR      (UART_BASE0 + 0x04) // Status and error clear register
#define UART0_FR      (UART_BASE0 + 0x18) // Flags register
#define UART0_IBRD    (UART_BASE0 + 0x24) // Integer baud rate
#define UART0_FBRD    (UART_BASE0 + 0x28) // Fractional baud rate
#define UART0_LCR_H   (UART_BASE0 + 0x28) // Line control registers (CTS/RTS)
#define UART0_CR      (UART_BASE0 + 0x30) // Control register
#define UART0_IMSC    (UART_BASE0 + 0x38) // Interrupt mask register

// FLAGS register at offset 8 - just two flags for polled I/O
// Bit 4 IS rxfe RX fifo empty
// Bit 5 IS txff TX fifo full flag


#define IOREAD32(ADDR)          (*((unsigned volatile int *)ADDR))
#define IOWRITE32(ADDR, WDATA)  { *((unsigned int *)ADDR) = WDATA; }
//-------------------------------------------------------------------
void cbg_uart_wrch(char cc)
{
  while (IOREAD32(UART0_FR)&(1<<5)) continue; // Spin on transmit FIFO full (TXFF flag set)
  IOWRITE32(UART0_DR, cc);
}
//-------------------------------------------------------------------
char cbg_uart_rdch (void)
{
  while (IOREAD32(UART0_FR)&(1<<4)) continue; // Spin while RX fifo is empty (RXFE flag set)
  return (IOREAD32(UART0_DR)) & 127;
}
//-------------------------------------------------------------------
void cbg_uart_init(void)
{
  IOWRITE32(UART0_IMSC, 0); // Disable all interrupts.
  IOWRITE32(UART0_IBRD, 0x04); // Select 115200 baud
  IOWRITE32(UART0_FBRD, 0x0);  // from a 7.37 MHz clock 
  IOWRITE32(UART0_LCR_H, 0x70); // 8-bits, FIFOs on, no parity, one stop bit.
  IOWRITE32(UART0_CR, 0x0F00);    // Enable RTS, DTD, RX and TX.
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
// Calling this function makes the SystemC, Prazor, GEM5 or QEMU simulation exit.
//
void emulator_exit_via_backdoor(char *msg, int return_value)
{
#ifdef OR1K  
  asm("l.addi\tr3,%0,0": :"r" (value));
  asm("l.nop %0": :"K" (NOP_EXIT)); // Exit backdoor instruction.
#elif 1

  // m5_exit()

  __asm__ __volatile__ ("mov r0, #0; mov r1, #0; .inst 0xEE000110 | (0x21 << 16);");

  // Somewhat dirty fallback: rely on seg fault from gem5 when reading this location to exit simulation.
  IOREAD32(UART_BASE0 + 0x8);
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
