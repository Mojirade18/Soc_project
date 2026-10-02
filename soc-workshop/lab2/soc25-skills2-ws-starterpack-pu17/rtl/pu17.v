//
//
// CBG PU17 microprocessor.
// (C) 1998-2024 University of Cambridge, Computer Laboratory.
//
// User resources: PC plus 8 G/P 16-bit registers.   Flags NCVZ. 
// Little-endian, byte addressed.  
// All registers G/P except R7 is used as base for ldm/stm and R6 is link register
//
// // $Id: pu17.v,v 1.4 2002/02/14 17:39:06 wrs Exp $
//
//
module PU17CORE(
		output [15:0] abus16, 
		input [15:0]  dbus16_in,
		output [15:0] dbus16_out,
		output reg    byteop, // For a byte store, abus16[0] is significant.
		input	      clk, 
		input	      reset,
		output	      opreq,
		output	      rwbar,
		input	      irq, 
		input	      waitb // Acts as a clock enable essentially
		);	
			
	// Wait should be changed so as to not glitch internal cycles ?


// Locals
  wire [15:0] pc, next_pc;
  wire [15:0] rbus, alubus, argbus;
  reg [15:0] ahold, lastr;
  wire branch_yes;			// A logic one when the branch condition holds.
  // Synchronise reset input
  reg sreset;
  always @(posedge clk) sreset <= reset;


  reg execute;			// Execute cycle
  reg internal;			// Internal cycle (when execute also needed)

  // Instruction decode wires
  reg update_flags;
  reg [3:0] branch_condition;
  reg regwen;
  reg [15:0] bdest;		// Branch destination
  reg [2:0] regnum;		// Register file read and write ports.

  reg write;
  reg byteopreq;
  reg imed8;
  reg argreq, argcycle;  
   reg linkf;			// Branch and link
   reg regind;			// Register indirect
   reg idx7;			// Even offsets to a base reg
   reg rlasave;			// High to save PC as a return address
   reg exreq;			// High to request an extension
   reg f0a,f0b,f0c, f1;		// Fetch0 and fetch 1 parts of inst
   reg last_cycle;		// End cycle of current instruction
   reg f1req;			// Request for second inst word
   reg abranch;
   reg [3:0] fc;		// ALU function code
   reg	     argislast;		// Used for reg to reg operations on single ported file
   reg	     multiple;		// Used for LDM/STM
   reg	     internal_req;
   reg [3:0] multiple_reg;	// current register to transfer in STM/LDM


  // Form a transparent latch for the old instruction.
  reg[15:0] ins_l;		 // Latched instruction opcode (use in f1 onwards to reduce combinatorial loops in net list).
  wire [15:0] ins = (f0a) ? dbus16_in: ins_l; // Always valid.
  always @(posedge clk) if (f0a) ins_l <= dbus16_in;

  wire advance = f0a | f1;
   CONTROL_UNIT pcm(pc, next_pc, advance, clk, waitb, reset, abranch, bdest);
   REGFILE rfile(.cen(waitb), .clk(clk), .regwen(regwen),
		 .rfile_in(alubus), .rfile_out(rbus), .regnum(regnum)
		 );

  assign dbus16_out = (rlasave) ? pc: (byteop) ? { rbus[7:0], rbus[7:0]} :rbus;
   
   
   // The ALU defaults to straight through on the B input, needing fc=12
   PUALU pualu(.y(alubus), .a(rbus), .b(argbus), .fc(fc), .clk(clk), .cen(waitb), 
	       .update_flags(update_flags), .branch_condition(branch_condition), 
	       .branch_yes(branch_yes));
   
   always @(posedge clk) if (sreset) begin
      f0a <= 0;
      f0b <= 0;
      f0c <= 0;
      f1 <= 0;
      argcycle <= 0;
      execute <= 0;
      internal <= 0;
      lastr <= 0;
      ahold <= 0;
   end
   else if (waitb) begin
      
      if (~execute & ~f0a & ~f1) 
	begin
	   f0a <= 1;	// start of day event.
	   f0b <= 1;	// start of day event.
	   f0c <= 1;	// start of day event.
	end
      else begin
	 f0a <= last_cycle;
	 f0b <= last_cycle;
	 f0c <= last_cycle;
      end
      
      f1 <= f1req;
      argcycle <= argreq;
      byteop <= byteopreq;
      execute <= exreq;
      if (f0a | f1) ahold <= dbus16_in;
      
      internal <= internal_req;
      
      // lastr is simply the register read the cycle before.
      if (!multiple) lastr <= rbus;
   end
   
   
   initial begin
      multiple = 0;
      update_flags = 0;
      branch_condition = 0;
      last_cycle = 0;
      update_flags = 0;
      rlasave = 0;
      
      imed8 = 0;
      write = 0;
      byteopreq = 0;
      regnum = 0;
      regwen = 0;
      argreq = 0;
      argcycle = 0;
      f1req = 0;	
      fc = 4'd12;	// ALU default to load mode
      argislast = 0;
      multiple = 0;
   end
   
   // Instruction decoder.
   always @(ins or ins_l or f1 or f0a or f0b or f0c or execute or alubus  or branch_condition or lastr
	    or multiple_reg or internal or pc or branch_yes or dbus16_in or fc) begin
      last_cycle = 0;
      fc = 4'd12;	// ALU default to load mode
      rlasave = 0;
      update_flags = 0;
      update_flags = 0;
      
      imed8 = 0;
      write = 0;
      regnum = 0;
      regwen = 0;
      argreq = 0;
      byteopreq = 0;
      f1req = 0;	
      linkf = 0;
      idx7 = 0;
      regind = 0;
      internal_req = 0;  // not used ?
      exreq = 0;
      argislast = 0;
      abranch = 0;
      bdest = 0;
      branch_condition = ins[5:2];
      multiple = 0;
      
      case(ins[15:12])
	
	4'h0, 4'h1, 4'h2, 4'h3, 4'h4, 4'h5, 4'h6, 4'h7:
	  // Arith/alu immed 8 bits, one cycle.
	  // If a shift, the immed arg is ignored and a shift of one is always done.
	  if (f0c) begin
	     last_cycle = 1;
	     fc = ins[6:3];
	     regnum = ins[9:7];
 	     regwen = (fc!=5 && fc!=13); // Not cmp or tst ;
	     update_flags = 1;
	     imed8 = 1;
	  end
	
	
	4'hA,
	  4'h8:	// Load from memory with index
	    begin
	       if (f0c) begin
		  regnum =  (ins[11:10]==3) ? 7: {1'b0, ins[11:10]};  // Read index reg to lastr in an internal cycle
		  exreq = 1;
	 	  byteopreq = ins[13];
		  argreq = 1;
	       end
	       if (execute) begin
		  regnum = ins_l[9:7];
		  last_cycle = 1;
		  regwen = 1;		// Indexed load with 6 bit offset
		  idx7 = 1;
	       end
	    end
	
	4'hB,
	  4'h9:	// Store to memory with index 
	    begin
	       if (f0c) begin
		  regnum =  (ins[11:10]==3) ? 7: {1'b0, ins[11:10]}; // Read index reg to lastr in an internal cycle
		  exreq = 1;
	 	  byteopreq = ins[13];
		  argreq = 1;
	       end
	       if (execute) begin
		  regnum = ins_l[9:7];
		  last_cycle = 1;
		  write = 1;
		  idx7 = 1;
	       end
	       
	    end
	
	4'hC:  // C is relative branch (BSR not supported)
	  begin
	     branch_condition = ins[11:8];		
	     abranch = branch_yes;
	     bdest = pc + { { 7 { ins[7] }}, ins[7:0], 1'b0 };
	     last_cycle = 1;
	  end
	
	4'hD:	
	  if (ins[11:10] == 2'b00) begin // D0 is arith reg, reg
	     fc = ins[6:3];
	     if (f0c) begin
		exreq = 1;	// Read reg on first cycle
		regnum = ins[2:0];
	     end
	     if (execute) begin
		regnum = ins_l[9:7];
		argislast = 1;
		last_cycle = 1;
		regwen = (fc!=5 && fc!=13); // Not cmp or tst 
		update_flags = 1;
	     end
 	  end
	
	  else if (ins[11:10] == 2'b01) begin // Load/store from memory abs 16
	     regnum = ins[9:7];
	     byteopreq = ins_l[6];
	     if (ins[5]==0) begin // Load from an abs 16 bit address 
		if (f0b) begin 
		   f1req = 1;
		end
		if (f1) begin
		   exreq = 1;
		   argreq = 1;
		end
		if (execute) begin
		   regwen = 1;
		   last_cycle = 1;
		end
	     end
	     
	     else// Store to memory abs 16
	       begin
		  regnum = ins[9:7];
		  if (f0b) begin 
		     f1req = 1;
		  end
		  if (f1) begin
		     exreq = 1;
		     argreq = 1;
		  end
		  if (execute) begin
		     write = 1;
		     last_cycle = 1;
		  end
	       end
	  end
	
	
	  else if (ins[11:10] == 2'b10) begin // D8, abs cond jump or link
	     if (f0c) f1req = 1;
	     if (f1) begin
		if (branch_condition == 15) begin // Branch with save of PC in r6
		   regnum = 6;
		   
		   regwen = 1;
		   linkf = 1;
		   abranch = 1;	
		end
		else abranch = branch_yes;
		bdest = dbus16_in;
		last_cycle = 1;
	     end		
	  end
	
	  else if (ins[11:10] == 2'b11) begin // LDM/STM
	     if (ins[1]) begin// store
		if (f0c) begin
		   f1req = 1;
		   regnum = 7;
		end
		if (f1) begin
		   f1req = multiple_reg != 8;
		   multiple = 1;
		   last_cycle = multiple_reg == 8;
		   write = 1;
		   regnum = multiple_reg;
		end
	     end
	     else begin // load
		if (f0c) begin
		   f1req = 1;
		   regnum = 7;
		end
		if (f1) begin
		   f1req = multiple_reg != 8;
		   multiple = 1;
		   last_cycle = multiple_reg == 8;
		   regwen = 1;
		   regnum = multiple_reg;
		end
	     end
	     
	     
	  end
	
	4'hF:  
	  
	  if (ins[11:10] == 0) begin
	     // F0 is register jump (used for ret) and bxl which is indirect branch and link
	     if (ins[0]) begin  // with link is two cycles 
		if (f0c) begin
		   exreq = 1;
		   regnum = ins[9:7];		
		end
		
		if (execute) begin
		   last_cycle = 1;
		   bdest = lastr;
		   regnum = 6;
		   linkf = 1;
		   regwen = 1;
		   abranch = 1;
		end
	     end	
	     
	     else begin // without link
		if (f0c) begin
		   regnum = ins[9:7];		
		   last_cycle = 1;
		   bdest = alubus;
		   abranch = 1;
		   fc = 0;		// function code 0 for reg unmodified
		end
	     end	
	  end
	
	
	  else if (ins[11:10] == 1) begin // F4 is load immediate 16 bit
	     regnum = ins[9:7];
	     if (f0c) begin
		f1req = 1;
	     end
	     if (f1) begin
		regwen = 1;
		last_cycle = 1;
	     end
	  end
	
      endcase
   end
   
   always @(posedge clk)
     if (0) begin
	$display(" PU17   pc=%h ins=%h exreq=%h execute=%h f0c=%h", pc, ins, exreq, execute, f0c);
	$display("        regnum=%h write=%h f1req=%h", regnum, write, f1req);
	$display("        argcycle=%h f0a=%h f1=%h", argcycle, f0a, f1);
	end

  assign rwbar = ~write;
  assign opreq = argcycle | f0a | f1;
  assign abus16 = 
		  (f0a|f1) ? pc: 				// Instruction fetch
		  (multiple) ? lastr + { multiple_reg, 1'b0 }:  // LDM STM
		  (idx7) ? lastr + { {8 {ins_l[5]}}, ins_l[6:0], 1'b0  }:	// 7 bit indexed addressing
		  (regind) ? lastr:			        // Register indirect
		  ahold;					// General absolute addresses
   
  //wire [15:0] testt =  { 10'b0+ins[5:0]};

  assign argbus = 
	(imed8) ? { 8'b0, ins[14:10], ins[2:0] }:
	(argislast) ? lastr: 
	(linkf) ? next_pc:
	(byteop & ~abus16[0]) ? { 8'h00, dbus16_in[7:0] }:  // Little endian
	(byteop & abus16[0]) ? { 8'h00, dbus16_in[15:8] }:
	dbus16_in;


  // LDM STM next register logic
  reg old_multiple;
  always @(posedge clk) begin
	old_multiple <= multiple;
	if (~old_multiple) begin
		multiple_reg <= (ins[2]) ? 0:
			(ins[3]) ? 1:
			(ins[4]) ? 2:
			(ins[5]) ? 3:
			(ins[6]) ? 4:
			(ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;
		end
	else case (multiple_reg)
		0: multiple_reg <= (ins[3]) ? 1:
			(ins[4]) ? 2:
			(ins[5]) ? 3:
			(ins[6]) ? 4:
			(ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;
		1: multiple_reg <= (ins[4]) ? 2:
			(ins[5]) ? 3:
			(ins[6]) ? 4:
			(ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;
		2: multiple_reg <= (ins[5]) ? 3:
			(ins[6]) ? 4:
			(ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;
		3: multiple_reg <= (ins[6]) ? 4:
			(ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;

		4: multiple_reg <= (ins[7]) ? 5:
			(ins[8]) ? 6:
			(ins[9]) ? 7: 8;

		5: multiple_reg <= (ins[8]) ? 6:
			(ins[9]) ? 7: 8;

		6: multiple_reg <= (ins[9]) ? 7: 8;

		7: multiple_reg <= 8;


		endcase
		
	end
  
endmodule
//
//
//====================================================================
//
// ALU, flags register and branch condition checker.
//
module PUALU(y, a, b, fc, clk, cen, update_flags, branch_condition, branch_yes);

  input [3:0] fc;		// Function code
  input [15:0] a, b;
  input clk, cen, update_flags;
  input [3:0] branch_condition;
  output branch_yes;

  reg carry, zero, negative, overflow;

  output [15:0] y;
  reg [15:0] y;
  wire [15:0] addsub;
 
  always @(a or b or fc or addsub) case (fc)
     0: y = a;  // straight through of register bus, used for store.
     1: y = addsub;
     2: y = addsub;
     3: y = addsub;
     4: y = addsub;
     5: y = addsub;                     // CMP
     6: y = a | b;
     7: y = a & b;
     8: y = a ^ b;
     9: y = a << 1;			// ASL/LSL
     10: y = { a[15], a[15:1] };  	// ASR
     11: y = a >> 1; 		 	// LSR
     12: y = b;                         // Used for mov/load
     13: y = a & b;                     // TST
   default : y = addsub;                // y = 16'bx;
   endcase

   wire n_carry;
   wire n_overflow;
   ADDSUB i_addsub(addsub, a, b, n_carry, carry, n_overflow, fc);


   always @(posedge clk) if (update_flags & cen) begin
	carry <= (fc==9)?a[15]: (fc==10)?a[0]: (fc==11)?a[0]: n_carry;
	zero <= (y==16'h0);
	negative <= y[15];
	overflow <= 0;
	end

   // These conditions follow exactly the 6800 processor.
   reg branch_yes;
   always @(branch_condition or carry or overflow or zero or negative) 
	case (branch_condition) 
		0:  branch_yes =  zero; // EQ
		1:  branch_yes =  ~zero; // NE
		2:  branch_yes = (negative ^ overflow); // LT
		3:  branch_yes = ~(negative ^ overflow) | zero; // GE
		4:  branch_yes = ~(negative ^ overflow) & ~zero; // GT 
		5:  branch_yes = (negative ^ overflow) | zero; // LE
		6:  branch_yes = carry;
		7:  branch_yes = ~carry;
		8:  branch_yes = overflow;
		9:  branch_yes = ~overflow;
		10:  branch_yes = 1;  // unconditional
		11:  branch_yes = ~carry & ~zero; // HI
		12:  branch_yes = carry | zero;   // LS
		13:  branch_yes = negative; // MI
		14:  branch_yes = ~negative; // PL
		default:  branch_yes = 1;  // Used for link
	endcase


endmodule 
//
//
//
//
module ADDSUB(addsub, a, b, n_carry, carry, n_overflow, fc);

  input [3:0] fc;		// Function code
  input [15:0] a, b;
  input carry;
  output [15:0] addsub;
  output n_overflow, n_carry;

//     1: y = a + b;
//     2: y = a - b;
//     3: y = a + b + carry;
//     4: y = a - b - carry;
//
//
   reg c;
   reg [15:0] bb;

   always @(fc or b or carry) case (fc)
      1: begin bb = b; c = 0; end
      
      default: begin bb = ~b; c = 1; end  // Subtract, compare and test.

      3: begin bb = b; c = carry; end
  
      4: begin bb = ~b; c = carry; end
    endcase

    wire [25:0] q, neta, netb;
    assign neta = { 8'b0, 1'b0, a, c };
    assign netb = { 8'b0, 1'b0, bb, c };
    ADDER26 adder26(q, neta, netb);
    assign n_carry = q[17]; // carry is in bit 18 if we had 1 in bit17 of netb
    assign addsub = q[16:1];

    wire msb_a = a[15];
    wire msb_bb = bb[15];
    
    assign n_overflow = (msb_a == msb_bb)  && (n_carry ^ q[16]);

endmodule
//
//
//
//
//
// The register file - single ported, but can read and write that register at the same time.
//
module REGFILE(
	       input	     clk, 
	       input	     cen,
	       input [15:0]  rfile_in,
	       output [15:0] rfile_out,
	       input [2:0]   regnum,
	       input	     regwen
	       );
  wire [15:0] y;
  wire wen = cen & regwen;
  // Write new data
  wire [15:0] nd =  rfile_in;
  // Write adderess
  wire [3:0] wa = { 1'b0, regnum };


`ifndef SYNTHESIS
  // Put this in for ease of tracing during behev simulation.
  reg [15:0] r0, r1, r2, r3, r4, r5, r6, r7;

  always @(posedge clk) begin
	if (wen && wa == 0) r0 <= nd;
	if (wen && wa == 1) r1 <= nd;
	if (wen && wa == 2) r2 <= nd;
	if (wen && wa == 3) r3 <= nd;
	if (wen && wa == 4) r4 <= nd;
	if (wen && wa == 5) r5 <= nd;
	if (wen && wa == 6) r6 <= nd;
	if (wen && wa == 7) r7 <= nd;
	end
  assign rfile_out =
	 (regnum == 0) ? r0:
	 (regnum == 1) ? r1:
	 (regnum == 2) ? r2:
	 (regnum == 3) ? r3:
	 (regnum == 4) ? r4:
	 (regnum == 5) ? r5:
	 (regnum == 6) ? r6:
	 r7;
`else
  assign rfile_out = y;


  // 16 words of RAM here, but use only first few for R0-7
  RAMS16x16 register_ram(y, nd, wen, clk, wa); 

`endif
   always @(posedge clk) begin
      if (wen) begin $display("%m:     r%0d <= 0x%h;", wa, nd); end
      end

endmodule
//
//
module RAMS16x8(y, nd, wen, clk, wa);

  output [7:0] y;
  input [7:0] nd;
  input wen, clk;
  input [3:0] wa;
  RAMS16x1 b0(y[0], nd[0], wen, clk, wa);
  RAMS16x1 b1(y[1], nd[1], wen, clk, wa);
  RAMS16x1 b2(y[2], nd[2], wen, clk, wa);
  RAMS16x1 b3(y[3], nd[3], wen, clk, wa);
  RAMS16x1 b4(y[4], nd[4], wen, clk, wa);
  RAMS16x1 b5(y[5], nd[5], wen, clk, wa);
  RAMS16x1 b6(y[6], nd[6], wen, clk, wa);
  RAMS16x1 b7(y[7], nd[7], wen, clk, wa);
endmodule
//
//
//
module RAMS16x16(y, nd, wen, clk, wa);

  output [15:0] y;
  input [15:0] nd;
  input wen, clk;
  input [3:0] wa;
  RAMS16x8 b0(y[7:0], nd[7:0], wen, clk, wa);
  RAMS16x8 b1(y[15:8], nd[15:8], wen, clk, wa);

endmodule
//
//
//
module CONTROL_UNIT(pc, next_pc, advance, clk, cen, sreset, abranch, bdest);

  input abranch;
  input [15:0] bdest;
  output [15:0] pc, next_pc;
  input clk, cen, sreset, advance;

  reg [15:0] pc;

  always @(posedge clk) 
  begin
     if (sreset) begin
	$display("%m CPU RESET");
	pc <= 0;
	end
     else if (cen & abranch) pc <= bdest; 
     else if (cen & advance) pc <= next_pc;

//#0 $display("    pc=%h bdest=%h cen=%h branch=%h advance=%h next_pc=%h", pc, bdest, cen, abranch, advance, next_pc);
  end

  assign next_pc = pc+2;
endmodule
//
//
// ----------------------------------------------------------
//
// EOF
//
