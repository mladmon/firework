// raw_data_out_fifo.v

// Generated using ACDS version 17.1 590

`timescale 1 ps / 1 ps
module raw_data_out_fifo (
		input  wire [7:0] data,  //  fifo_input.datain
		input  wire       wrreq, //            .wrreq
		input  wire       rdreq, //            .rdreq
		input  wire       clock, //            .clk
		input  wire       sclr,  //            .sclr
		output wire [7:0] q,     // fifo_output.dataout
		output wire       full,  //            .full
		output wire       empty  //            .empty
	);

	raw_data_out_fifo_fifo_171_u2h7cyi fifo_0 (
		.data  (data),  //  fifo_input.datain
		.wrreq (wrreq), //            .wrreq
		.rdreq (rdreq), //            .rdreq
		.clock (clock), //            .clk
		.sclr  (sclr),  //            .sclr
		.q     (q),     // fifo_output.dataout
		.full  (full),  //            .full
		.empty (empty)  //            .empty
	);

endmodule
