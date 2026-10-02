// SoC-25 CBG PU17 microprocessor testbench
//
// (C) 1998-2024 University of Cambridge, Computer Laboratory.
//
// pu17_testbench.v
//
module SIMSYS();
   reg clk;
   reg reset;
   reg [23:0] counter; // Counts clock cycles.

// `ifdef TRC   
   initial begin
      $dumpfile ("vcd.vcd"); 
      $dumpvars(1, SIMSYS); 
   end
// `endif   
   initial begin  // Clock and reset generator.
      clk = 0;
      reset = 1;
      counter = 0;
      $display("Hello World from the SoC-25 PU17 testbench.\n");
      while (counter < 1000) begin
	 #50 clk = !clk;
	 if (clk) counter = counter + 1;
	 if (counter > 3) reset <= #2 0;
      end
      $finish;
   end
   
   
   // Local Nets - the bus nets.
   wire [15:0] uart_data16, rom_data16, ram_data16, cpu_writed16; // Data bus
   wire [15:0] abus16;  // Address bus
   wire	       waitb = 1;
   wire	       opreq;   // Operation request
   wire	       rwbar;   // Read=1, Write=0.
   wire	       wbyte;   // High for a byte store.
   wire	       irq = 0; // Interrupt request - not being used.


   
   // CPU core instance
   PU17CORE the_cpu_core(
			 .clk(clk), 
			 .reset(reset),
			 .opreq(opreq),
			 .rwbar(rwbar),
			 .irq(irq), 
			 .byteop(wbyte), 
			 .abus16(abus16),
			 .dbus16_in(cpu_readd16),
			 .dbus16_out(cpu_writed16),
			 .waitb(waitb) 
			 );
   // Bootrom instance
   CANNED_PUTEST_ROM bootom(.data(rom_data16), .addr(abus16[15:1]));   

   // 16K word (32 Kbyte) of 16-bit RAM.
   wire	ram_sel = (abus16[15:14] == 1 || abus16[15:14] == 2) & opreq; // Address base of RAM is zero. 
   wire lane0 = !wbyte || (abus16[0] == 0); // Low byte lane decode for 8-bit store to 16-bit words.
   wire lane1 = !wbyte || (abus16[0] == 1); // Ditto high.
   // Asynch read, sync write.
   CBG_SSRAM0_16K_X_16 main_memory(.clk(clk),
				   .rwbar(rwbar),
				   .sel(ram_sel),
				   .addr(abus16[14:1]),
				   .lane0(lane0), .lane1(lane1),
				   .write_data(cpu_writed16),
				   .read_data(ram_data16));
   

   
   // UART instance.
   wire	uart_sel = (abus16[15:4] == 12'hF88) & opreq; // Address base 0xF880 for uart selection
   UART16 i_uart16(.serin(serin), .serout(serout), .baudclk16(baudclk16),
		   .dbusout(uart_data16), .dbusin(cpu_writed16), .addr(abus16[3:1]), .sel(uart_sel), 
		   .clk(clk), .rwbar(rwbar));

   // Read data bus multiplexor.
   wire [15:0] cpu_readd16 = (ram_sel) ? ram_data16: (uart_sel)? uart_data16: rom_data16;
   

   // Bus monitor
   always @(posedge clk) $display("%t  opreq=%h rwbar=%1d  A=%h din=%h  dout=%h", $time, opreq, rwbar, abus16, cpu_readd16, cpu_writed16);

   // Backdoor - simulator exit
   always @(posedge clk) 
     if (uart_sel && !rwbar && cpu_writed16 == 16'h004B) begin
	$display("%t  Backdoor finish sequence detected. UART 'K' character written.", $time);
	# 400 $finish;
     end
			
endmodule
// EOF
