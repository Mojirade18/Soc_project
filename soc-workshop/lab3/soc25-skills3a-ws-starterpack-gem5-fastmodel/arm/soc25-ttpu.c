//
// SoC-25 example TTPM (Toy Trusted Platform Module)
//  
// This example demonstrates a generic setup for a hardware accelerator
// or trusted platform. It is much simpler than a standard 11889-compliant
// implementation, but it demonstrates the EDA considerations that typically apply.
// 
// Hand-crafted Fast, C, cycle-callable model.


// OneTimePad_512x17 OneTimeROM  --- Secret key/stream information ROM prototype.
unsigned int OneTimePad_512x17(unsigned char addr);

#if 1
#include "soc25-ttpu.h"
#else
// This struct is now in the .h file:
typedef struct 
{
  unsigned int /*reg [19:0]*/	     income_counter;  // Timer for next bus credit unit
  unsigned char /*reg [3:0]*/	     credit_counter;  // Credit available to peform bus cycles
  unsigned char /*reg [6:0]*/	     hidden_state;    // Unresettable and unknowable state
  unsigned short /*reg [15:0]*/	     outdata_reg;     // Data stored ready for host
  unsigned char /*bool */            badf;            // Bad operation flag
} SOC25_TTPU101_state;
#endif  


void SOC25_TTPU101_eval(
			SOC25_TTPU_CBG_state *ss,
			unsigned char /*bool*/ reset, // active high reset
			unsigned char /*bool*/ rwbar, // Read/write bar
			unsigned char /*bool*/ sel, // Select
			unsigned char /*[1:0]*/   addr, // Address input
			unsigned int  /*[31:0]*/  dbus_in, // Data bus in  
			unsigned int  /*[31:0]*/ *dbus_out_p	// Data out
			)

{
  if (reset) {
    ss->credit_counter = 15;
    ss->income_counter = 0;
    ss->badf = 0;
  }
  else {
    ss->income_counter = (ss->income_counter + 1) & ((1<<19)-1);
    if (ss->income_counter == 0 && ss->credit_counter < 15) ss->credit_counter = (ss->credit_counter + 1) & ((1<<4)-1);
    if (sel && !rwbar && addr==1 && ss->credit_counter == 0) ss->badf = 1;
    else if (sel && !rwbar && addr==1)
      {	    
	ss->credit_counter = ss->credit_counter - 1;
	unsigned int /*[16:0]*/ rom_dout = OneTimePad_512x17(ss->hidden_state);
	ss->outdata_reg = (rom_dout & ((1<<16)-1)) ^ (dbus_in & ((1<<16)-1));
	unsigned char /*bool*/  dhash = (dbus_in & 0x00001111) ==  0x00001111;
	ss->badf = 0;
	if (dhash || ((rom_dout>>16) & 1)) ss->hidden_state = (ss->hidden_state + 1) & ((1<<7)-1);
      }
  }
   
   
  unsigned int /*[31:0]*/ dbus_out = // Output multiplexor - implements programmers' view register space: four 32-bit words.
      ((addr & 3) == 1) ? (ss->badf<<31) | ss->outdata_reg:
      ((addr & 3) == 2) ? ss->credit_counter:  // Read off the credit counter
    0x0000A5A4;  // Otherwise a device identifier cum filler is returned.

  if (dbus_out_p) *dbus_out_p = dbus_out;
}




// Contents to be securely mask programmed into tamper-proof/obfuscated logic.
unsigned int OneTimePad_512x17(unsigned char addr)
{
  int data = -1;
  switch (addr)
    {
    case 0x00: data = 0x07341; break;
    case 0x01: data = 0x132F4; break;
    case 0x02: data = 0x09072; break;
    case 0x03: data = 0x05061; break;
    case 0x04: data = 0x13974; break;
    case 0x05: data = 0x172E5; break;
    case 0x06: data = 0x03320; break;
    case 0x07: data = 0x07661; break;
    case 0x08: data = 0x139EC; break;
    case 0x09: data = 0x037A0; break;
    case 0x0A: data = 0x1906E; break;
    case 0x0B: data = 0x13474; break;
    case 0x0C: data = 0x15065; break;
    case 0x0D: data = 0x037E8; break;
    case 0x0E: data = 0x179F5; break;
    case 0x0F: data = 0x15665; break;
    case 0x10: data = 0x03A20; break;
    case 0x11: data = 0x032E8; break;
    case 0x12: data = 0x03320; break;
    case 0x13: data = 0x1FAEF; break;
    case 0x14: data = 0x09072; break;
    case 0x15: data = 0x0F463; break;
    case 0x16: data = 0x07669; break;
    case 0x17: data = 0x13964; break;
    case 0x18: data = 0x17765; break;
    case 0x19: data = 0x030A0; break;
    case 0x1A: data = 0x0B2F2; break;
    case 0x1B: data = 0x039A0; break;
    case 0x1C: data = 0x17765; break;
    case 0x1D: data = 0x11074; break;
    case 0x1E: data = 0x137F4; break;
    case 0x1F: data = 0x029A0; break;
    case 0x20: data = 0x17AED; break;
    case 0x21: data = 0x1F3E7; break;
    case 0x22: data = 0x132EC; break;
    case 0x23: data = 0x093F2; break;
    case 0x24: data = 0x0D073; break;
    case 0x25: data = 0x137D4; break;
    case 0x26: data = 0x01670; break;
    case 0x27: data = 0x03A20; break;
    case 0x28: data = 0x032E8; break;
    case 0x29: data = 0x03420; break;
    case 0x2A: data = 0x1F6EF; break;
    case 0x2B: data = 0x15065; break;
    case 0x2C: data = 0x1F36F; break;
    case 0x2D: data = 0x026A0; break;
    case 0x2E: data = 0x09072; break;
    case 0x2F: data = 0x132CC; break;
    case 0x30: data = 0x1B7EE; break;
    case 0x31: data = 0x07969; break;
    case 0x32: data = 0x1102C; break;
    case 0x33: data = 0x05061; break;
    case 0x34: data = 0x1B2E6; break;
    case 0x35: data = 0x1366C; break;
    case 0x36: data = 0x1FBEF; break;
    case 0x37: data = 0x179AD; break;
    case 0x38: data = 0x0F4E3; break;
    case 0x39: data = 0x17765; break;
    case 0x3A: data = 0x134F4; break;
    case 0x3B: data = 0x0FA73; break;
    case 0x3C: data = 0x037A0; break;
    case 0x3D: data = 0x19066; break;
    case 0x3E: data = 0x1F2C7; break;
    case 0x3F: data = 0x1F96F; break;
    case 0x40: data = 0x1F2E7; break;
    case 0x41: data = 0x1F9A7; break;
    case 0x42: data = 0x03320; break;
    case 0x43: data = 0x07A61; break;
    case 0x44: data = 0x032E8; break;
    case 0x45: data = 0x09672; break;
    case 0x46: data = 0x02AA0; break;
    case 0x47: data = 0x1B1EE; break;
    case 0x48: data = 0x132EC; break;
    case 0x49: data = 0x028A0; break;
    case 0x4A: data = 0x172F5; break;
    case 0x4B: data = 0x1BA6E; break;
    case 0x4C: data = 0x07769; break;
    case 0x4D: data = 0x1902E; break;
    case 0x4E: data = 0x0F6D3; break;
    case 0x4F: data = 0x173F5; break;
    case 0x50: data = 0x1F667; break;
    case 0x51: data = 0x17965; break;
    case 0x52: data = 0x1F9A7; break;
    case 0x53: data = 0x02A20; break;
    case 0x54: data = 0x1F86F; break;
    case 0x55: data = 0x034A0; break;
    case 0x56: data = 0x0D073; break;
    case 0x57: data = 0x05061; break;
    case 0x58: data = 0x07AF1; break;
    case 0x59: data = 0x172E5; break;
    case 0x5A: data = 0x09072; break;
    case 0x5B: data = 0x037E8; break;
    case 0x5C: data = 0x179F5; break;
    case 0x5D: data = 0x15065; break;
    case 0x5E: data = 0x07A61; break;
    case 0x5F: data = 0x03A20; break;
    case 0x60: data = 0x032E8; break;
    case 0x61: data = 0x039A0; break;
    case 0x62: data = 0x176F5; break;
    case 0x63: data = 0x174ED; break;
    case 0x64: data = 0x11074; break;
    case 0x65: data = 0x1F36F; break;
    case 0x66: data = 0x030A0; break;
    case 0x67: data = 0x1906E; break;
    case 0x68: data = 0x1F66F; break;
    case 0x69: data = 0x11064; break;
    case 0x6A: data = 0x034E8; break;
    case 0x6B: data = 0x1366C; break;
    case 0x6C: data = 0x137F4; break;
    case 0x6D: data = 0x01070; break;
    case 0x6E: data = 0x0F7E3; break;
    case 0x6F: data = 0x079E1; break;
    case 0x70: data = 0x130F4; break;
    case 0x71: data = 0x1106C; break;
    case 0x72: data = 0x137F4; break;
    case 0x73: data = 0x1F777; break;
    case 0x74: data = 0x1102C; break;
    case 0x75: data = 0x030F0; break;
    case 0x76: data = 0x0BA72; break;
    case 0x77: data = 0x13CEC; break;
    case 0x78: data = 0x039A0; break;
    case 0x79: data = 0x17975; break;
    case 0x7A: data = 0x0B7F2; break;
    case 0x7B: data = 0x17775; break;
    case 0x7C: data = 0x132E4; break;
    case 0x7D: data = 0x11064; break;
    case 0x7E: data = 0x0BCE2; break;
    case 0x7F: data = 0x03220; break;
    }
  return data;
}

// eof
