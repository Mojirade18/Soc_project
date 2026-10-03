// CBG uart16.cv
//
// (C) 1998-2025 DJ Greaves, University of Cambridge, Computer Laboratory.
//
//
//  9600 baud uart. Fixed data format:
//  0: uartstatus (address 0x80)
//       Bit 0 is txidle
//       Bit 1 is rxav
//  1: uartdata   (address 0x82)
//
//
module UART16( // baudclk16, serin, serout, dbusin, dbusout, addr, sel, clk, rwbar
	       input baudclk16,
	       input [15:0] dbusin,
	       output [15:0] dbusout,
	       input clk,
	       input [2:0] addr,		// PU16 address bits 3 to 1.
	       input sel,
	       input rwbar,
	       input serin,
	       output serout
	       );
   
   wire		      tx_guard = sel & (addr[0] == 1) & ~rwbar;
   wire		      rx_guard = sel & (addr[0] == 1) & rwbar;
   wire		      read = rx_guard;
   wire		      statusread = (addr[0] == 0);
   
   wire [7:0]	      txdata = dbusin[7:0];
   
   wire [7:0]	      rxhold;
   wire		      rxav;
   wire		      txidle;
   
   STOP stop(clk, baudclk16, serin, rxhold, read, rxav);
   
   assign dbusout = (statusread) ? { 14'b0, rxav, txidle }
		    : { 8'b0, rxhold }; 
   
   PTOS ptos(clk, baudclk16, serout, txdata, tx_guard, txidle);
endmodule

//
//
// UARTRX : Serial-to-parallel convertor
module STOP(clk, baudclk16, serin, pardata, read, rxav);
  input clk;
  input serin;        // RX data, polarity is such that start bit is a zero.
  output [7:0] pardata;   
  output rxav;
  input read;
  input baudclk16;

  //-----------------------------
  // Composite input decision flip-flop, synchronises the input and
  // deletes short, spurious start bits
  reg dec_a, dec_b, dec;
  initial dec_a = 0;
  initial dec_b = 0;
  initial dec = 0;
  always @(posedge baudclk16)
	begin
	dec_a <= serin;
	dec_b <= dec_a;
	dec <= (dec_a & dec_b) | (dec & ~(dec_a | dec_b));
	end

  //-----------------------------
  // Input sequencer.
  // 
  // The sequencer starts at 0 and counts 16 baudclk periods per bit
  // for a total of 9.5 bits (152 cycles).
  //
  //
  reg [3:0] rseq;		 initial rseq = 0;
  reg running;			 initial running = 0;
  reg [3:0] phase;		 initial phase = 0;

  reg [3:0] endgap_counter;	 initial endgap_counter = 0;

  wire startbit = ~running & ~dec;
  wire sample = (rseq == 4'd6);

  wire false_start = sample & (phase == 0) & dec;

  wire stopbit = sample & (phase == 4'd9);
  // e next_is_stopbit = sample & (phase == 4'd8);

  always @(posedge baudclk16)
	begin
	if (startbit && endgap_counter == 0) running <= 1;
	if (stopbit | false_start) running <= 0;

	rseq <= (running) ? rseq+1'd1: 4'd0;

	if (sample | ~running) phase <= (running) ? phase + 4'd1: 4'd0;

	if (stopbit) endgap_counter <= 5;

	if (endgap_counter > 0) endgap_counter <= endgap_counter - 1;

	end

  //---------------------------------------
  // Serial to parallel conversion
  // Shift into msb to handle lsb first.

    reg [7:0] rxshift;  initial rxshift = 0;
    always @(posedge baudclk16) if (sample) rxshift <= { dec, rxshift[7:1] };

  //---------------------------------------
  // Output holding register an data guard

  reg [7:0] pardata;
  reg rxav;
  always @(posedge clk) begin
	if (read) rxav <= 0;

	if (stopbit & ~baudclk16) begin
		pardata <= rxshift;
		rxav <= 1;
		end

	end
endmodule
//
//
//
// Parallel to serial convertor, data sending.
//
module PTOS(clk, baudclk16, serout, pardata, guard, txidle);
  input clk;
  input baudclk16;
  input guard;		// High to request a read and transmit cycle
  output serout;
  input [7:0] pardata;
  output txidle;  	// High to request new data to send
  wire reset = 0;  	// Not used reset signal



//----------------------------------------------------------
// Free running divide by 16 to get actual baud rate clock
  reg  [3:0] t_div16;	initial t_div16 = 0;
  always @(posedge baudclk16) t_div16 <= t_div16 + 4'd1;
  wire txck = t_div16[3];
//----------------------------------------------------------
// Input holding register and clock domain crossing

  reg [7:0] txhold;
  always @(posedge clk) 
    if (guard) begin
       //$display("%m: TX CHAR output %c", pardata & 16'hff);
       txhold <= pardata;
       end
  wire localidle;	// High when not transmitting 
  wire end_limit;	// High at end of a transmission.

  
// Generator for the start signal - unnecessarily asynchronous
  reg start; 
  initial start = 0;
  always @(posedge clk) begin
	if (guard) start <= 1;
	if (end_limit) start <= 0;
	end


  // Deassert external idle signal if starting or running.
  assign txidle = localidle & ~start;
  
//----------------------------------------------------------
// Divide by ten tx framer - stays at zero when idle.

  reg [3:0] txframe;	initial txframe = 0;
  
  assign localidle = (txframe == 4'd0);

  wire move = start | ~localidle;
  assign end_limit = (txframe == 4'd9);
  always @(posedge txck) if (move) txframe <= (end_limit) ? 4'd0 : txframe + 4'd1;

//----------------------------------------------------------
// Serial-to-parallel.  Shift right to send lsb first

  reg [8:0] txshift;
  wire [8:0] newd;
  assign newd[8:1] = txhold;
  assign newd[0] = 0;  // This is the start bit.
  always @(posedge txck) 
	begin
	txshift = (start & localidle) ? newd: { 1'd1, txshift[8:1] };
	if (reset) txshift = 9'h1FF;

	end

  assign serout = txshift[0] | localidle | reset;

  

endmodule
//
// eof

