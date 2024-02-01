

module signal_interrupt
#(  parameter MSB   = 8,
    parameter PULL  = 1)
(
    input clk,
    input rst,
    input clr,
    input en,
    input [MSB-1:0] in,

    output reg intr
);

    reg [MSB-1:0] r_in;
    wire          w_flip = (r_in ^ in) > 0;

    always @(posedge clk) begin
        if(~rst) begin
            intr    <= 1'b1;
            if(PULL)
                r_in <= {(MSB){1'b1}};
            else
                r_in <= {(MSB){1'b0}};
        end
        else if(~clr) begin
            r_in    <= in;
            intr    <= 1'b1;
        end
        else if(en) begin
            r_in    <= in;
            intr    <= intr & ~w_flip;
        end
    end

endmodule
