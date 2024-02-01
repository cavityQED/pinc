
module debounce
#(  parameter TICK  = 1000000,
    parameter MSB   = 8)
(
    input   clk,
    input   rst,
    input   en,
    input   in,
    input   pull,

    output  rise,
    output  fall,
    output  out
);
    parameter   CUR = MSB-2;
    parameter   PRE = MSB-1;

    wire        w_edge;
    reg         r_busy;
    reg [31:0]  r_count;
    reg [PRE:0] r_in;

    assign w_edge   = r_in[PRE] ^ r_in[CUR];
    assign rise     = w_edge & r_in[CUR];
    assign fall     = w_edge & ~r_in[CUR];
    assign out      = r_in[CUR];

    initial
        r_in <= {(MSB){pull}};

    always @(posedge clk) begin
        if(~rst) begin
            r_count <= 0;
            r_in    <= {(MSB){pull}};
        end
        else if(en) begin
            if(r_busy) begin
                if(r_count == TICK) begin
                    r_count <= 0;
                    r_busy  <= 1'b0;
                end
                else begin
                    r_count <= r_count + 1'b1;
                end
            end
            else if(w_edge) begin
                r_busy  <= 1'b1;
                r_in    <= {(MSB){r_in[CUR]}};
            end
            else begin
                r_in    <= {r_in[CUR:0], in};
            end
        end
    end

endmodule
