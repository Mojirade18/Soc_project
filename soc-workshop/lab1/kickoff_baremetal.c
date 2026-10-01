//
// SoC-25 GEM5/QEMU Skills Video. (C) 2025, DJ Greaves, University of Cambridge, Computer Laboratory.
// kickoff_baremetal.c

// Uart interface - routines exported by the UART device driver.
extern void cbg_uart_init();
extern void cbg_uart_wrch(char);
extern void cbg_uart_puts(char *);
extern char cbg_uart_rdch();

//-------------------------------------------------------------------
// Print a number in hexadecimal with given field wid
static void writex(int field, unsigned int dd)
{
  field *= 4;
  while(field > 0)
    {
      field -= 4;
      unsigned int  c0 = (dd >> field) & 0xF;
      unsigned char cc = (c0>9) ? c0 + 'A'-10: c0 + '0';
      cbg_uart_wrch(cc);
    }
  cbg_uart_wrch(0x20);
}
  
//-------------------------------------------------------------------
extern int mymon_main(char *msg);

int notmain(void)
{
  cbg_uart_init();
  cbg_uart_puts("Hello from kickoff_baremetal.c\n");

  // print table of squares
  
  for (int i=0; i<12; i++)
    {
      for (int j=0; j<12; j++) { writex(3, i*j); 	cbg_uart_wrch(' '); }
      cbg_uart_wrch('\r');
      cbg_uart_wrch('\n');
    }

  mymon_main(__FILE__);
  
  return(0);
}

// eof
