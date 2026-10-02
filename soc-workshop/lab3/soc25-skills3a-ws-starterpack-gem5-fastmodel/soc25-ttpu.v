//
// SoC-25 example TTPM (Toy Trusted Platform Module)
//  
// This example demonstrates a generic setup for a hardware accelerator
// or trusted platform. It is much simpler than a standard 11889-compliant
// implementation, but it demonstrates the EDA considerations that typically apply.
// 
// Verilog RTL version.


module SOC25_TTPU101(
       input	     clk,   // clock
       input	     reset, // active high reset
       input	     rwbar, // Read/write bar
       input	     sel,   // Select
       input [1:0]   addr,  // Address input
       input [31:0]  dbus_in,   // Data bus in for writes.
       output [31:0] dbus_out	// Data out to host.
       );


   reg [19:0]	     income_counter;  // Timer for next bus credit unit
   reg [3:0]	     credit_counter;  // Credit available to peform bus cycles
   reg [6:0]	     hidden_state;    // Unresettable and unknowable state
   reg [15:0]	     outdata_reg;     // Data stored ready for host
   reg		     badf;            // Bad operation flag
   wire [16:0]	     rom_dout;
   
   OneTimePad_512x17 OneTimeROM(  // Secret key/stream information ROM
       				  .addr(hidden_state),
				  .dout(rom_dout));

   wire		     dhash = (dbus_in & 32'h0000_1111) ==  32'h0000_1111;
   
   always @(posedge clk) begin
      $display("%m: %1t clocked", $time);
      if (reset) begin
	 credit_counter = 15;
	 income_counter = 0;
	 badf = 0;
      end
      else begin
	 income_counter = income_counter + 1;
	 if (income_counter == 0 && credit_counter < 15) credit_counter = credit_counter + 1;
	 if (sel && !rwbar && addr==1 && credit_counter == 0) badf = 1;
	 else if (sel && !rwbar && addr==1) begin	    
	    credit_counter = credit_counter - 1;
	    outdata_reg = rom_dout[15:0] ^ dbus_in[15:0];
	    badf = 0;
	    if (dhash || rom_dout[16]) hidden_state = hidden_state + 1;
	 end
      end
   end // always @ (posedge clk)
   
   assign dbus_out = // Output multiplexor - implements programmers' view register space
      		     (addr == 1) ? { badf, 15'h0, outdata_reg }:
		     (addr == 2) ? { 28'h0, credit_counter }:  // Read off the credit counter
		     32'h0000A5A4;  // Otherwise a device identifier cum filler is returned.
   
endmodule




// Contents to be securely mask programmed into tamper-proof/obfuscated logic.
module OneTimePad_512x17(  
       			   input [6:0]	 addr,
			   output[16:0] dout);
   reg [16:0]				     data;
   assign dout = data;
   
   always @(addr)
     case (addr) 
       7'h00 : data = 17'h07341;
       7'h01 : data = 17'h132F4;
       7'h02 : data = 17'h09072;
       7'h03 : data = 17'h05061;
       7'h04 : data = 17'h13974;
       7'h05 : data = 17'h172E5;
       7'h06 : data = 17'h03320;
       7'h07 : data = 17'h07661;
       7'h08 : data = 17'h139EC;
       7'h09 : data = 17'h037A0;
       7'h0A : data = 17'h1906E;
       7'h0B : data = 17'h13474;
       7'h0C : data = 17'h15065;
       7'h0D : data = 17'h037E8;
       7'h0E : data = 17'h179F5;
       7'h0F : data = 17'h15665;
       7'h10 : data = 17'h03A20;
       7'h11 : data = 17'h032E8;
       7'h12 : data = 17'h03320;
       7'h13 : data = 17'h1FAEF;
       7'h14 : data = 17'h09072;
       7'h15 : data = 17'h0F463;
       7'h16 : data = 17'h07669;
       7'h17 : data = 17'h13964;
       7'h18 : data = 17'h17765;
       7'h19 : data = 17'h030A0;
       7'h1A : data = 17'h0B2F2;
       7'h1B : data = 17'h039A0;
       7'h1C : data = 17'h17765;
       7'h1D : data = 17'h11074;
       7'h1E : data = 17'h137F4;
       7'h1F : data = 17'h029A0;
       7'h20 : data = 17'h17AED;
       7'h21 : data = 17'h1F3E7;
       7'h22 : data = 17'h132EC;
       7'h23 : data = 17'h093F2;
       7'h24 : data = 17'h0D073;
       7'h25 : data = 17'h137D4;
       7'h26 : data = 17'h01670;
       7'h27 : data = 17'h03A20;
       7'h28 : data = 17'h032E8;
       7'h29 : data = 17'h03420;
       7'h2A : data = 17'h1F6EF;
       7'h2B : data = 17'h15065;
       7'h2C : data = 17'h1F36F;
       7'h2D : data = 17'h026A0;
       7'h2E : data = 17'h09072;
       7'h2F : data = 17'h132CC;
       7'h30 : data = 17'h1B7EE;
       7'h31 : data = 17'h07969;
       7'h32 : data = 17'h1102C;
       7'h33 : data = 17'h05061;
       7'h34 : data = 17'h1B2E6;
       7'h35 : data = 17'h1366C;
       7'h36 : data = 17'h1FBEF;
       7'h37 : data = 17'h179AD;
       7'h38 : data = 17'h0F4E3;
       7'h39 : data = 17'h17765;
       7'h3A : data = 17'h134F4;
       7'h3B : data = 17'h0FA73;
       7'h3C : data = 17'h037A0;
       7'h3D : data = 17'h19066;
       7'h3E : data = 17'h1F2C7;
       7'h3F : data = 17'h1F96F;
       7'h40 : data = 17'h1F2E7;
       7'h41 : data = 17'h1F9A7;
       7'h42 : data = 17'h03320;
       7'h43 : data = 17'h07A61;
       7'h44 : data = 17'h032E8;
       7'h45 : data = 17'h09672;
       7'h46 : data = 17'h02AA0;
       7'h47 : data = 17'h1B1EE;
       7'h48 : data = 17'h132EC;
       7'h49 : data = 17'h028A0;
       7'h4A : data = 17'h172F5;
       7'h4B : data = 17'h1BA6E;
       7'h4C : data = 17'h07769;
       7'h4D : data = 17'h1902E;
       7'h4E : data = 17'h0F6D3;
       7'h4F : data = 17'h173F5;
       7'h50 : data = 17'h1F667;
       7'h51 : data = 17'h17965;
       7'h52 : data = 17'h1F9A7;
       7'h53 : data = 17'h02A20;
       7'h54 : data = 17'h1F86F;
       7'h55 : data = 17'h034A0;
       7'h56 : data = 17'h0D073;
       7'h57 : data = 17'h05061;
       7'h58 : data = 17'h07AF1;
       7'h59 : data = 17'h172E5;
       7'h5A : data = 17'h09072;
       7'h5B : data = 17'h037E8;
       7'h5C : data = 17'h179F5;
       7'h5D : data = 17'h15065;
       7'h5E : data = 17'h07A61;
       7'h5F : data = 17'h03A20;
       7'h60 : data = 17'h032E8;
       7'h61 : data = 17'h039A0;
       7'h62 : data = 17'h176F5;
       7'h63 : data = 17'h174ED;
       7'h64 : data = 17'h11074;
       7'h65 : data = 17'h1F36F;
       7'h66 : data = 17'h030A0;
       7'h67 : data = 17'h1906E;
       7'h68 : data = 17'h1F66F;
       7'h69 : data = 17'h11064;
       7'h6A : data = 17'h034E8;
       7'h6B : data = 17'h1366C;
       7'h6C : data = 17'h137F4;
       7'h6D : data = 17'h01070;
       7'h6E : data = 17'h0F7E3;
       7'h6F : data = 17'h079E1;
       7'h70 : data = 17'h130F4;
       7'h71 : data = 17'h1106C;
       7'h72 : data = 17'h137F4;
       7'h73 : data = 17'h1F777;
       7'h74 : data = 17'h1102C;
       7'h75 : data = 17'h030F0;
       7'h76 : data = 17'h0BA72;
       7'h77 : data = 17'h13CEC;
       7'h78 : data = 17'h039A0;
       7'h79 : data = 17'h17975;
       7'h7A : data = 17'h0B7F2;
       7'h7B : data = 17'h17775;
       7'h7C : data = 17'h132E4;
       7'h7D : data = 17'h11064;
       7'h7E : data = 17'h0BCE2;
       7'h7F : data = 17'h03220;
     endcase
endmodule // OneTimePad_512x17


// eof;
