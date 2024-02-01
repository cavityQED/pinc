

module status_register
#(parameter MSB = 7)    
(
    input   clk,
    input   rst,
    input   en,
    output  reg intr,

    input       [MSB:0] in,
    output reg  [MSB:0] out
);

    wire diff = (in ^ out) > 0;

    initial begin
        intr    <= 1'b1;
    end

    always @(posedge clk) begin
        if(~rst) begin
            intr    <= 1'b1;
        end
        else begin
            out <= in;
            if(en) begin
                intr <= intr & ~diff;
            end
        end
    end

endmodule
