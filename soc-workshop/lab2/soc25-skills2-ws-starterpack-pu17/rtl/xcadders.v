//
// (C) 1998-2024 University of Cambridge, Computer Laboratory.
//
//
// Xilinx hardmacro library for use with cv2 compilers
// Simple adders using carry logic
//

/* primitive */ module ADDER26(q, neta, netb);
  output [25:0] q;
  input [25:0] neta, netb;
  assign #5  q = neta + netb;
endmodule

/* primitive */ module ADDER24(q, neta, netb);
  output [23:0] q;
  input [23:0] neta, netb;
  assign #5  q = neta + netb;
endmodule

/* primitive */ module ADDER18(q, neta, netb);
  output [17:0] q;
  input [17:0] neta, netb;
  assign #3  q = neta + netb;
endmodule

/* primitive */ module SUBTRACT18(q, neta, netb);
  output [17:0] q;
  input [17:0] neta, netb;
  assign #3  q = neta - netb;
endmodule


/* primitive */ module ADDER18_REG(q, neta, netb, clk, ce);
  output [17:0] q;
  input [17:0] neta, netb;
  input clk, ce;  
  reg [17:0] q;
  always @(posedge clk) if (ce) q <= #3 neta + netb;
endmodule

//eof
  
