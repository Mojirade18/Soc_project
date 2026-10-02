#ifndef SOC25_TTPU_H
#define SOC25_TTPU_H

//
// SoC-25 example TTPM (Toy Trusted Platform Module) 
//  
// This example demonstrates a generic setup for a hardware accelerator
// or trusted platform. It is much simpler than a standard 11889-compliant
// implementation, but it demonstrates the EDA considerations that typically apply.
// 
// Hand-crafted Fast, C, cycle-callable model.


#define SOC25_TTPU_CHIP_ID 0xc0c0A5A4

struct SOC25_TTPU_CBG_state
{
  unsigned int /*reg [19:0]*/	   income_counter;  // Timer for next bus credit unit
  unsigned char /*reg [3:0]*/	   credit_counter;  // Available credit to peform bus cycles
  unsigned char /*reg [6:0]*/	   hidden_state;    // Unresettable and unknowable state
  unsigned short /*reg [15:0]*/    outdata_reg;     // Data stored ready for host
  unsigned char /*bool */          badf;            // Bad operation flag
}; 


extern void SOC25_TTPU101_eval(
			SOC25_TTPU_CBG_state *ss,
			unsigned char /*bool*/   reset, // active high reset
			unsigned char /*bool*/   rwbar, // Read/write bar
			unsigned char /*bool*/   sel, // Select
			unsigned char /*[1:0]*/  addr, // Address input
			unsigned int  /*[31:0]*/ dbus_in, // Data bus in  
			unsigned int  /*[31:0]*/*dbus_out_p	// Data out
			       );



#endif
