
module edge_detect
(
    input   clk,
    input   rst,
    input   in,
    input   pull,
    output  flip,
    output  rise,
    output  fall
);
    reg     r_in;

    assign  flip = r_in ^ in;
    assign  rise = flip & in;
    assign  fall = flip & ~in;

    initial
        r_in <= pull;

    always @(posedge clk) begin
        if(~rst)
            r_in <= pull;
        else
            r_in <= in;
    end

endmodule

