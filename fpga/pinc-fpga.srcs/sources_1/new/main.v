

module top
(
    input           clk,
    input           rst,
    
    input           spi_clk,
    input           spi_cs,
    input           spi_mosi,
    output          spi_miso,
    
    input [7:0]     button_status,
    input [3:0]     pico_status,
    
    output [2:0]    intr
);

    reg         spi_tx_valid;
    reg  [7:0]  spi_tx_byte;
    wire [7:0]  spi_rx_byte;
    wire        spi_tx_ready;
    wire        spi_rx_valid;

    spi spi0 
    (
        clk,
        rst,

        spi_clk,
        spi_cs,
        spi_mosi,
        spi_miso,

        spi_tx_byte,
        spi_rx_byte,

        spi_tx_valid,
        spi_tx_ready,
        spi_rx_valid,
    );

    assign intr[2] = 1'b1;

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

    wire [7:0] p_rise;
    wire [7:0] p_fall;
    wire [7:0] p_out;
    debounce #(.TICK(10)) pico_db[7:0]
    (clk, rst, 1'b1, {4'hF, pico_status}, 1'b1, p_rise, p_fall, p_out);

    reg  b_clr;
    reg  b_en;
    signal_interrupt    b_sig   (clk, rst, b_clr, b_en, b_out, intr[0]);
    
    reg p_clr;
    reg p_en;
    signal_interrupt    p_sig   (clk, rst, p_clr, p_en, p_out, intr[1]);

    initial begin
        spi_tx_byte <= 8'h00;
        b_en        <= 1'b1;
        p_en        <= 1'b1;
    end

    always @(posedge clk) begin
        spi_tx_valid    <= 1'b0;
        b_clr           <= 1'b1;
        p_clr           <= 1'b1;

        if(spi_tx_ready) begin
            case (intr)
                3'b110  : spi_tx_byte <= b_out;
                3'b101  : spi_tx_byte <= p_out;
            endcase
            spi_tx_valid <= 1'b1;
        end

        if(spi_cs_rise) begin
            b_clr   <= intr[0];
            p_clr   <= intr[1];
        end
    end

endmodule   