

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
    parameter X_STATUS_REG = 8'hA0;
    parameter Y_STATUS_REG = 8'hB0;

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
    wire b_int_new;
    signal_interrupt    b_sig   (clk, rst, b_clr, b_en, b_out, b_int_new, intr[0]);
    
    reg x_clr;
    reg x_en;
    wire x_int_new;
    signal_interrupt    x_sig   (clk, rst, x_clr, x_en, x_out, x_int_new, intr[1]);

    reg y_clr;
    reg y_en;
    wire y_int_new;
    signal_interrupt    y_sig   (clk, rst, y_clr, y_en, y_out, y_int_new, intr[2]);

    reg         data_rw;
    reg [7:0]   data_addr;
    reg [7:0]   rd_addr;
    reg [7:0]   wr_byte;
    reg         wr_valid;
    wire [7:0]  rd_byte;
    wire        rd_valid;
    data_register   spi_data_bank   (clk, rst, data_rw, data_addr, wr_byte, wr_valid, rd_byte, rd_valid);

    reg x_int_reg;
    reg x_int_fall;
    reg y_int_reg;
    reg y_int_fall;

    initial begin
        x_int_reg <= 1'b1;
        y_int_reg <= 1'b1;
    end

    always @(posedge clk) begin
        x_int_reg   <= intr[1];
        x_int_fall  <= ~intr[1] & (intr[1] ^ x_int_reg);
        y_int_reg   <= intr[2];
        y_int_fall  <= ~intr[2] & (intr[2] ^ y_int_reg);
    end

    reg [7:0] spi_byte_num;

    initial begin
        spi_tx_byte     <= 8'h00;
        spi_byte_num    <= 8'h00;
        b_en            <= 1'b1;
        x_en            <= 1'b1;
        y_en            <= 1'b1;
    end


    // SPI MISO
    // High impedance (disconnected) when spi inactive (cs high)
    always @(posedge clk) begin
        if(spi_cs)
            spi_miso_reg    <= 1'bz;
        else
            spi_miso_reg    <= spi_miso_wire;
    end

    always @(posedge clk) begin
        data_rw         <= 1'b0;
        spi_tx_valid    <= 1'b0;
        wr_valid        <= 1'b0;
        b_clr           <= 1'b1;
        x_clr           <= 1'b1;
        y_clr           <= 1'b1;

        if(spi_cs_rise) begin
            spi_byte_num    <= 8'h00;
            case (rd_addr)
                X_STATUS_REG    : x_clr <= 1'b0;
                Y_STATUS_REG    : y_clr <= 1'b0;
            endcase
        end

        if(spi_rx_valid) begin
            if(spi_byte_num == 8'h00) begin
                rd_addr     <= spi_rx_byte;
                data_addr   <= spi_rx_byte;
                data_rw     <= 1'b1;
            end
            spi_byte_num    <= spi_byte_num + 1'b1;
        end

        if(rd_valid) begin
            spi_tx_byte     <= rd_byte;
        end

        if(spi_tx_ready) begin
            spi_tx_valid    <= 1'b1;
        end

        if(x_int_new) begin
            data_addr       <= X_STATUS_REG;
            wr_byte         <= x_out;
            wr_valid        <= 1'b1;
        end
        else if(y_int_new) begin
            data_addr       <= Y_STATUS_REG;
            wr_byte         <= y_out;
            wr_valid        <= 1'b1;
        end
    end

endmodule   