module data_register
(
	input 				clk,	// clock
	input 				rst,	// reset
	input				rw,		// read/write

	input		[7:0]	addr,

	input		[7:0]	wr_byte,
	input				wr_valid,

	output	reg	[7:0]	rd_byte,
	output	reg			rd_valid			
);

	reg	[7:0] data [255:0];

	always @(posedge clk) begin
		rd_valid		<= 1'b0;
		if(rw) begin					// read
			rd_byte		<= data[addr];
			rd_valid	<= 1'b1;
		end
		else if(~rw & wr_valid) begin	// write
			data[addr]	<= wr_byte;
		end
	end

endmodule