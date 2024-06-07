

module top
(
    input           clk,
    input           rst,
    
    input           spi_clk,
    input           spi_cs,
    input           spi_mosi,
    output          spi_miso,

    input [3:0]     x_status,
    input [3:0]     y_status,

    output [7:0]    out,
    output [2:1]    intr
);

    localparam X_STATUS_REG     = 8'hA0;
    localparam Y_STATUS_REG     = 8'hB0;

    wire            x_signal;
    reg             x_clr;
    wire [3:0]      x_sync;
    wire [3:0]      x_db;
    sync x0[3:0]    (clk, rst, x_status, x_sync);
    db   x1[3:0]    (clk, rst, x_sync, x_db);
    signal_interrupt #(.MSB(4)) x2 (clk, rst, x_clr, 1'b1, x_db, x_signal, intr[1]);

    wire            y_signal;
    reg             y_clr;
    wire [3:0]      y_sync;
    wire [3:0]      y_db;
    sync y0[3:0]    (clk, rst, y_status, y_sync);
    db   y1[3:0]    (clk, rst, y_sync, y_db);
    signal_interrupt #(.MSB(4)) y2 (clk, rst, y_clr, 1'b1, y_db, y_signal, intr[2]);

    reg [7:0]   data_addr;
    reg [7:0]   data_rd_addr;
    reg         data_read;
    reg [7:0]   data_in;
    reg         data_wr_valid;
    wire [7:0]  data_out;
    wire        data_rd_valid;

    data_register spi_reg (clk, rst, data_read, data_addr, data_in, data_wr_valid, data_out, data_rd_valid);

    wire spi_clk_sync;
    wire spi_clk_rise;
    wire spi_clk_fall;
    wire spi_clk_db;
    sync #(.PULL(1'b0)) clk_sync    (clk, rst, spi_clk, spi_clk_sync);
    db                  clk_db      (clk, rst, spi_clk_sync, spi_clk_db);

    wire spi_cs_sync;
    wire spi_cs_db;
    wire spi_cs_rise;
    wire spi_cs_fall;
    sync #(.PULL(1'b1)) cs_sync     (clk, rst, spi_cs, spi_cs_sync);
    db                  cs_db       (clk, rst, spi_cs_sync, spi_cs_db);
    edges               cs_edges    (clk, rst, spi_cs_db, spi_cs_rise, spi_cs_fall);

    wire spi_mosi_sync;
    sync #(.PULL(1'b0)) mosi_sync   (clk, rst, spi_mosi, spi_mosi_sync);

    wire        rx_valid;
    wire        tx_ready;
    reg         tx_valid;
    reg  [7:0]  tx_byte;
    wire [7:0]  rx_byte;
    reg  [7:0]  spi_byte_num;
    wire        spi_miso_wire;
    reg         spi_miso_reg;

    spi spi0
    (
        clk,
        rst,

        spi_clk_db,
        spi_cs_db,
        spi_mosi_sync,
        spi_miso_wire,

        tx_byte,
        rx_byte,

        tx_valid,
        tx_ready,
        rx_valid
    );

    assign out[0]   = x_db[0];
    assign out[1]   = x_db[1];
    assign out[2]   = x_db[2];
    assign out[3]   = x_db[3];
    assign spi_miso = spi_miso_reg;

    initial begin
        tx_byte         <= 8'h00;
        tx_valid        <= 1'b0;
        spi_byte_num    <= 8'h00;
    end

    always @(posedge clk) begin
        if(spi_cs_db) begin
            spi_miso_reg    <= 1'bz;
        end
        else begin
            spi_miso_reg    <= spi_miso_wire;
        end
    end

    always @(posedge clk) begin
        tx_valid        <= 1'b0;
        data_read       <= 1'b0;
        data_wr_valid   <= 1'b0;
        x_clr           <= 1'b1;
        y_clr           <= 1'b1;

        if(spi_cs_rise) begin
            spi_byte_num    <= 8'h00;
            case (data_rd_addr)
                X_STATUS_REG    : x_clr <= 1'b0;
                Y_STATUS_REG    : y_clr <= 1'b0;
            endcase
        end

        if(rx_valid) begin
            if(spi_byte_num == 8'h00) begin
                data_rd_addr    <= rx_byte;
                data_addr       <= rx_byte;
                data_read       <= 1'b1;
            end
            spi_byte_num        <= spi_byte_num + 1'b1;
        end

        if(data_rd_valid) begin
            tx_byte         <= data_out;
        end

        if(tx_ready) begin
            tx_valid        <= 1'b1;
        end

        if(x_signal) begin
            data_addr       <= X_STATUS_REG;
            data_wr_valid   <= 1'b1;
            data_in         <= {4'hF, x_db};
        end
        else if(y_signal) begin
            data_addr       <= Y_STATUS_REG;
            data_wr_valid   <= 1'b1;
            data_in         <= {4'hF, y_db};
        end
    end

endmodule   