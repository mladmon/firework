// raw_data_in_wstrb.v

// Generated using ACDS version 16.1 196

`timescale 1 ps / 1 ps
module raw_data_in_wstrb (
		input  wire [3:0] data,  //  fifo_input.datain
		input  wire       wrreq, //            .wrreq
		input  wire       rdreq, //            .rdreq
		input  wire       clock, //            .clk
		input  wire       sclr,  //            .sclr
		output wire [3:0] q      // fifo_output.dataout
	);

	raw_data_in_wstrb_fifo_161_el7h66y fifo_0 (
		.data  (data),  //  fifo_input.datain
		.wrreq (wrreq), //            .wrreq
		.rdreq (rdreq), //            .rdreq
		.clock (clock), //            .clk
		.sclr  (sclr),  //            .sclr
		.q     (q)      // fifo_output.dataout
	);

endmodule
