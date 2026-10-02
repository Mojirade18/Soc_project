// 
// XROMGEN OUTPUT EUCLIDTEST_ROM.v
// 
module CANNED_PUTEST_ROM(data, addr);   
 
  output [15:0] data; 
  reg [15:0] data; 
  input [4:0] addr; 
 
  always @(addr) case (addr)
   0:  data = 16'h18E2; 
   1:  data = 16'hF500; 
   2:  data = 16'h20D; 
   3:  data = 16'hD129; 
   4:  data = 16'hD814; 
   5:  data = 16'h12; 
   6:  data = 16'hD1E1; 
   7:  data = 16'hD0E2; 
   8:  data = 16'hD163; 
   9:  data = 16'hD129; 
   10:  data = 16'hD800; 
   11:  data = 16'h1C; 
   12:  data = 16'hD092; 
   13:  data = 16'hCAF6; 
   14:  data = 16'hD4A0; 
   15:  data = 16'hF882; 
   16:  data = 16'h2567; 
   17:  data = 16'hD520; 
   18:  data = 16'hF882; 
   19:  data = 16'h2563; 
   20:  data = 16'hD520; 
   21:  data = 16'hF882; 
   22:  data = 16'hCA00; 
  default : data = 0; // change to X
  endcase 
endmodule 
// 
