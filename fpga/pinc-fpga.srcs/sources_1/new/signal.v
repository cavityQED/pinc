module dff
#(	parameter PULL = 1'b1)
(
	input		clk,
	input		rst,
	input		d,
	
	output reg	q
);

	always @(posedge clk) begin
		if(~rst)
			q	<= PULL;
		else
			q	<= d;
	end

endmodule



module sync
#(  parameter PULL = 1'b1)
(
	input		clk,
	input		rst,
	input		in,

	output reg	out
);

	wire q1, q2;

	dff #(.PULL(PULL)) dff1(clk, rst, in, q1);
	dff #(.PULL(PULL)) dff2(clk, rst, q1, q2);

	always @(posedge clk) begin
		if(~rst)
			out	<= PULL;
		else
			out <= q2;
	end

endmodule



module db
#(	parameter TICK	= 5)
(
	input		clk,
	input		rst,
	input		in,

	output reg	out
);
	
	localparam	cbits = $clog2(TICK);

	reg				r_counting;
	reg [cbits:0]	r_count;

	always @(posedge clk) begin
		if(~rst) begin
			r_count		<= 0;
			r_counting	<= 1'b0;
			out			<= in;
		end
		else begin
			if(r_counting) begin
				if(r_count == TICK) begin
					r_count		<= 0;
					r_counting	<= 1'b0;
					out			<= in;
				end
				else
					r_count		<= r_count + 1'b1;
			end
			else begin
				if(out ^ in) begin
					r_counting	<= 1'b1;
				end
				else
					out	<= in;
			end
		end
	end

endmodule



module edges
(
	input		clk,
	input		rst,
	input		in,

	output	reg	rise,
	output	reg fall
);

	reg		prev;

	initial begin
		prev 	<= in;
		rise	<= 1'b0;
		fall	<= 1'b0;
	end

	always @(posedge clk) begin
		if(~rst) begin
			prev	<= in;	
			rise	<= 1'b0;
			fall	<= 1'b0;
		end
		else begin
			prev	<= in;
			rise	<= (prev ^ in) & in;
			fall	<= (prev ^ in) & ~in;
		end
	end

endmodule



module signal
#(	parameter 			MSB		= 7,
	parameter 			TICK	= 5,
	parameter [MSB:0]	PULL	= {(MSB){1'b1}})
(
	input				clk,
	input				rst,
	input				en,
	input				clr,
	input 		[MSB:0]	in,
	output	reg	[MSB:0]	out,
	output	reg			flip,
	output	reg			intr
);

	wire [MSB:0]	in_sync, in_db;
	wire			flip_wire = (in_db ^ out) > 0;
	
	sync			sync_mod[MSB:0]	(clk, rst, in, in_sync);
	db				db_mod[MSB:0]	(clk, rst, in_sync, in_db);

	always @(posedge clk) begin
		if(~rst) begin
			out		<= in_db;
			flip	<= 1'b0;
			intr	<= 1'b1;	
		end
		else if(en) begin
			out		<= in_db;
			flip	<= flip_wire;
			intr	<= ~clr | (intr & ~flip_wire);
		end
	end

endmodule