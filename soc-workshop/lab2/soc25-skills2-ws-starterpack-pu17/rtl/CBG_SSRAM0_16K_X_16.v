//
// (C) 1998-2024 University of Cambridge, Computer Laboratory.
//

// Asynchronous-read (latency 0), synchronous write.
module CBG_SSRAM0_16K_X_16 (
			    input	  clk,
			    input	  rwbar,
			    input	  sel,
			    input [13:0]  addr, // 14 bits of address (lsb only used for byte writes).
			    input	  lane0, // Two byte lanes
			    input	  lane1,
			    input [15:0]  write_data,
			    output [15:0] read_data
			    );


   reg [7:0]					      lo_array [16535:0];
   reg [7:0]					      hi_array [16535:0];   


   // Asynchronous read
   assign read_data[7:0 ] = lo_array[addr[13:0]];
   assign read_data[15:8] = hi_array[addr[13:0]];      
      
   always @(posedge clk) begin // Synchronous write
      if (sel && !rwbar) begin
	if (lane0) lo_array[addr[13:0]] <= write_data[7:0];
	if (lane1) hi_array[addr[13:0]] <= write_data[15:8];	 
	 end
   end
   
endmodule

// eof   
