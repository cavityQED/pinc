

// msb first shift register
module shift_register
#(parameter MSB = 7)    
(
    input   clk,
    input   rst,
    input   en,
    input   in,

    output reg [MSB:0]  out
);
    initial
        out <= 0;

    always @(posedge clk) begin
        if(~rst) begin
            out <= 0;
        end else begin
            if(en)
                out <= {out[MSB-1:0], in};
        end
    end

endmodule
