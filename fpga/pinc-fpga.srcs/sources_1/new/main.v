

module top
(
    input           clk,
    input           rst,
    
    input           spi_clk,
    input           spi_cs,
    input           spi_mosi,
    output          spi_miso,
    
    input [7:0]     button_status,
    input [3:0]     x_status,
    input [3:0]     y_status,
    
    output [2:0]    intr
);

    reg         spi_tx_valid;
    reg  [7:0]  spi_tx_byte;
    wire [7:0]  spi_rx_byte;
    wire        spi_tx_ready;
    wire        spi_rx_valid;

    reg         spi_miso_reg;
    wire        spi_miso_wire;
    assign      spi_miso = spi_miso_reg;

    spi spi0 
    (
        clk,
        rst,

        spi_clk,
        spi_cs,
        spi_mosi,
        spi_miso_wire,

        spi_tx_byte,
        spi_rx_byte,

        spi_tx_valid,
        spi_tx_ready,
        spi_rx_valid,
    );

    wire spi_cs_rise;
    wire spi_cs_fall;
    wire spi_cs_flip;
    wire spi_cs_out;
    debounce #(.TICK(10), .MSB(4)) cs_db
    (clk, rst, 1'b1, spi_cs, 1'b0, spi_cs_rise, spi_cs_fall, spi_cs_out);

    wire [7:0] b_rise;
    wire [7:0] b_fall;
    wire [7:0] b_out;
    debounce button_db[7:0]
    (clk, rst, 1'b1, button_status, 1'b1, b_rise, b_fall, b_out);

    wire [7:0] x_rise;
    wire [7:0] x_fall;
    wire [7:0] x_out;
    debounce #(.TICK(10)) x_db[7:0]
    (clk, rst, 1'b1, {4'hF, x_status}, 1'b1, x_rise, x_fall, x_out);

    wire [7:0] y_rise;
    wire [7:0] y_fall;
    wire [7:0] y_out;
    debounce #(.TICK(10)) y_db[7:0]
    (clk, rst, 1'b1, {4'hF, y_status}, 1'b1, y_rise, y_fall, y_out);

    reg  b_clr;
    reg  b_en;
    signal_interrupt    b_sig   (clk, rst, b_clr, b_en, b_out, intr[0]);
    
    reg x_clr;
    reg x_en;
    signal_interrupt    x_sig   (clk, rst, x_clr, x_en, x_out, intr[1]);

    reg y_clr;
    reg y_en;
    signal_interrupt    y_sig   (clk, rst, y_clr, y_en, y_out, intr[2]);

    initial begin
        spi_tx_byte <= 8'h00;
        b_en        <= 1'b1;
        x_en        <= 1'b1;
        y_en        <= 1'b1;
    end

    always @(posedge clk) begin
        spi_tx_valid    <= 1'b0;
        b_clr           <= 1'b1;
        x_clr           <= 1'b1;
        y_clr           <= 1'b1;
        spi_miso_reg    <= 1'bz;

        if(spi_tx_ready) begin
            case (intr)
                3'b110  : spi_tx_byte <= b_out;
                3'b101  : spi_tx_byte <= x_out;
                3'b011  : spi_tx_byte <= y_out;
            endcase
            spi_tx_valid <= 1'b1;
        end

        if(spi_cs_rise) begin
            b_clr   <= intr[0];
            x_clr   <= intr[1];
            y_clr   <= intr[2];
        end
        else if(~spi_cs) begin
            spi_miso_reg    <= spi_miso_wire;
        end
    end

endmodule   