module varint_encoder_top ( /* Implements AMBA AXI4 slave interface */
		input  wire        clock_clk,      //  clock.clk
		input  wire        reset_reset,    //  reset.reset

		input  wire [3:0]  axs_s0_awid,    // axs_s0.awid
		input  wire [31:0] axs_s0_awaddr,  //       .awaddr
		input  wire [7:0]  axs_s0_awlen,   //       .awlen
		input  wire [2:0]  axs_s0_awsize,  //       .awsize
		input  wire [1:0]  axs_s0_awburst, //       .awburst
		input  wire        axs_s0_awvalid, //       .awvalid
		output wire        axs_s0_awready, //       .awready

		input  wire [31:0] axs_s0_wdata,   //       .wdata
		input  wire [3:0]  axs_s0_wstrb,   //       .wstrb
		input  wire        axs_s0_wvalid,  //       .wvalid
		output wire        axs_s0_wready,  //       .wready

		input  wire        axs_s0_bready,  //       .bready
		output wire [3:0]  axs_s0_bid,     //       .bid
		output wire        axs_s0_bvalid,  //       .bvalid

		input  wire [3:0]  axs_s0_arid,    //       .arid
		input  wire [31:0] axs_s0_araddr,  //       .araddr
		input  wire [7:0]  axs_s0_arlen,   //       .arlen
		input  wire [2:0]  axs_s0_arsize,  //       .arsize
		input  wire [1:0]  axs_s0_arburst, //       .arburst
		input  wire        axs_s0_arvalid, //       .arvalid
		output wire        axs_s0_arready, //       .arready

		output wire [3:0]  axs_s0_rid,     //       .rid
		output wire [31:0] axs_s0_rdata,   //       .rdata
		output wire        axs_s0_rlast,   //       .rlast
		output wire        axs_s0_rvalid,  //       .rvalid
		input  wire        axs_s0_rready   //       .rready
	);
	
	// Internal wires
	wire        in_empty, in_full, in_pop, in_push;
	wire [31:0] in_q;
	wire        varint_in_sel, varint_clr, varint_ld, gt_eq_128, varint_out_sel;
	wire        raw_data_clr, raw_data_ld;
	wire [1:0]  raw_data_sel;
	wire        byte_sel;
	wire        out_empty, out_full, out_pop, out_push;
	wire [7:0]  out_data;
	wire        fifo_clr;
	
	// Submodule instances
	in_fifo in0 (
		.data  (axs_s0_wdata),           //  fifo_input.datain
		.wrreq (in_push),                //            .wrreq
		.rdreq (in_pop),                 //            .rdreq
		.clock (clock_clk),              //            .clk
		.sclr  (fifo_clr),               //            .sclr
		.q     (in_q),                   // fifo_output.dataout
		.full  (in_full),                //            .full
		.empty (in_empty)                //            .empty
	);
	
	controller c0 (
		.clk            (clock_clk),
		.reset          (reset_reset),
		.axs_s0_awid    (axs_s0_awid),
		.axs_s0_awaddr  (axs_s0_awaddr),
		.axs_s0_awlen   (axs_s0_awlen),
		.axs_s0_awsize  (axs_s0_awsize),
		.axs_s0_awburst (axs_s0_awburst),
		.axs_s0_awvalid (axs_s0_awvalid),
		.axs_s0_awready (axs_s0_awready),
		.axs_s0_wstrb   (axs_s0_wstrb),
		.axs_s0_wvalid  (axs_s0_wvalid),
		.axs_s0_wready  (axs_s0_wready),
		.axs_s0_bready  (axs_s0_bready),
		.axs_s0_bid     (axs_s0_bid),
		.axs_s0_bvalid  (axs_s0_bvalid),
		.axs_s0_arid    (axs_s0_arid),
		.axs_s0_araddr  (axs_s0_araddr),
		.axs_s0_arlen   (axs_s0_arlen),
		.axs_s0_arsize  (axs_s0_arsize),
		.axs_s0_arburst (axs_s0_arburst),
		.axs_s0_arvalid (axs_s0_arvalid),
		.axs_s0_arready (axs_s0_arready),
		.axs_s0_rready  (axs_s0_rready),
		.axs_s0_rid     (axs_s0_rid),
		.axs_s0_rlast   (axs_s0_rlast),
		.axs_s0_rvalid  (axs_s0_rvalid),
		.in_empty       (in_empty),
		.in_full        (in_full),
		.in_pop         (in_pop),
		.in_push        (in_push),
		.varint_in_sel  (varint_in_sel),
		.varint_clr     (varint_clr),
		.varint_ld      (varint_ld),
		.varint_out_sel (varint_out_sel),
		.gt_eq_128      (gt_eq_128),
		.raw_data_clr   (raw_data_clr),
		.raw_data_ld    (raw_data_ld),
		.raw_data_sel   (raw_data_sel),
		.byte_sel       (byte_sel),
		.out_empty      (out_empty),
		.out_full       (out_full),
		.out_pop        (out_pop),
		.out_push       (out_push),
		.fifo_clr       (fifo_clr)
	);
	
	datapath d0 (
		.clk            (clock_clk),
		.reset          (reset_reset),
		.varint_data    (in_q),
		.raw_data       (axs_s0_wdata),
		.out            (out_data),
		.varint_in_sel  (varint_in_sel),
		.varint_clr     (varint_clr),
		.varint_ld      (varint_ld),
		.varint_out_sel (varint_out_sel),
		.gt_eq_128      (gt_eq_128),
		.raw_data_clr   (raw_data_clr),
		.raw_data_ld    (raw_data_ld),
		.raw_data_sel   (raw_data_sel),
		.byte_sel       (byte_sel)
	);
	
	out_fifo out0 (
		.data  (out_data),               //  fifo_input.datain
		.wrreq (out_push),               //            .wrreq
		.rdreq (out_pop),                //            .rdreq
		.clock (clock_clk),              //            .clk
		.sclr  (fifo_clr),               //            .sclr
		.q     (axs_s0_rdata[7:0]),      // fifo_output.dataout
		.full  (out_full),               //            .full
		.empty (out_empty)               //            .empty
	);
	
	assign axs_s0_rdata[31:8] = 24'b000000000000000000000000;

endmodule


/*module varint_encoder_top (
		input  wire			clock_clk,		 //	clock.clk
		input  wire			reset_reset		 //	reset.reset

		// Avalon-ST sink interface
		input  wire [31:0] asi_in0_data,	 // asi_in0.data
		input  wire        asi_in0_valid, //		  .valid
		output wire        asi_in0_ready, // 		  .ready
	);

endmodule

// Avalon-ST sink
`timescale 1 ps / 1 ps
module new_component (
        input  wire [31:0] asi_in0_data,  // asi_in0.data
        output wire        asi_in0_ready, //        .ready
        input  wire        asi_in0_valid, //        .valid
        input  wire        clock_clk,     //   clock.clk
        input  wire        reset_reset    //   reset.reset
    );

    // TODO: Auto-generated HDL template
    assign asi_in0_ready = 1'b0;

endmodule */
