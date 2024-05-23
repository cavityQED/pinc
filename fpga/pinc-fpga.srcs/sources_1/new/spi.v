

module spi
#(  parameter MSB = 7)
(
    input               clk,
    input               rst,

    input               spi_clk,
    input               spi_cs,
    input               spi_mosi,
    output reg          spi_miso,
    
    input       [7:0]   tx_byte,
    output reg  [7:0]   rx_byte,
    
    input               tx_valid,
    output reg          tx_ready, 
    output reg          rx_valid,

    output              clk_out
);

    reg [7:0] r_tx_byte;
    reg [2:0] r_tx_bit;
    reg [2:0] r_rx_bit;
    reg [4:0] r_edges;

    wire   w_clk;
    wire   w_clk_rise;
    wire   w_clk_fall;
    wire   w_clk_edge = w_clk_rise | w_clk_fall;
    assign clk_out = w_clk;

    reg    spi_clk_rst;
    reg    spi_clk_en;

    debounce #(.TICK(5)) spi_clk_db
    (
        clk,
        rst,
        spi_clk_en,
        spi_clk,
        1'b0,

        w_clk_rise,
        w_clk_fall,
        w_clk
    );

    // clock
    always @(posedge clk) begin
        if(spi_cs) begin
            r_edges     <= 2*(MSB+1);
            spi_clk_en  <= 1'b0;
        end
        else begin
            spi_clk_en  <= 1'b1;
            if(tx_valid) begin
                r_edges <= 2*(MSB+1);
            end
            else if(r_edges > 0) begin
                if(w_clk_edge) begin
                    r_edges <= r_edges - 1'b1;
                end
            end
        end
    end

    // miso
    always @(posedge clk) begin
        tx_ready  <= 1'b0;
        if(tx_valid) begin
            r_tx_byte   <= tx_byte;
            r_tx_bit    <= MSB;
        end
        if(spi_cs) begin
            r_tx_bit    <= MSB;
            r_tx_byte   <= 0;
            spi_miso  <= 1'b0;
        end
        else begin
            if(r_edges == 16) begin
                spi_miso  <= r_tx_byte[MSB];
                r_tx_bit    <= MSB - 1'b1;
            end
            else if(w_clk_fall) begin
                spi_miso  <= r_tx_byte[r_tx_bit];
                if(r_tx_bit > 0) begin
                    r_tx_bit    <= r_tx_bit - 1'b1;
                end
                if(r_edges == 1) begin
                    tx_ready  <= 1'b1;
                end
            end
        end
    end

    // mosi
    always @(posedge clk) begin
        rx_valid  <= 1'b0;
        if(tx_valid) begin
            r_rx_bit    <= MSB;
        end
        if(spi_cs) begin
            r_rx_bit    <= MSB;
        end
        else begin
            if(w_clk_rise) begin
                rx_byte[r_rx_bit] <= spi_mosi;
                if(r_rx_bit > 0) begin
                    r_rx_bit    <= r_rx_bit - 1'b1;
                end
                else begin
                    rx_valid  <= 1'b1;
                end
            end
        end
    end

    
endmodule