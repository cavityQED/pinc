

module top
(
    input           clk,
    input           rst,
    
    input           spi_clk,
    input           spi_cs,
    input           spi_mosi,
    output          spi_miso,

    input [3:0]     x_status,
    input           x_limit,
    input           x_en_in,
    input           x_dir_in,
    input           x_step_in,
    output          x_alarm,
    output          x_en_out,
    output          x_dir_out,
    output          x_step_out,
    output          x_limit_com,

    input [3:0]     y_status,
    input           y_limit,
    input           y_en_in,
    input           y_dir_in,
    input           y_step_in,
    output          y_alarm,
    output          y_en_out,
    output          y_dir_out,
    output          y_step_out,
    output          y_limit_com,

    input           wheel_a,
    input           wheel_b,

    output [7:0]    out,
    output [2:0]    intr
);

    wire       x_limit_sync;
    wire       x_limit_db;
    sync x3    (clk, rst, x_limit, x_limit_sync);
    db #(.TICK(200000))   x4    (clk, rst, x_limit_sync, x_limit_db);

    wire       y_limit_sync;
    wire       y_limit_db;
    sync y3    (clk, rst, y_limit, y_limit_sync);
    db #(.TICK(200000))   y4    (clk, rst, y_limit_sync, y_limit_db);

    wire all_limits = x_limit_db & y_limit_db;
    wire step_alarm = ~all_limits;

    assign x_alarm      = step_alarm;
    assign x_en_out     = x_en_in;
    assign x_dir_out    = x_dir_in;
    assign x_step_out   = x_step_in & all_limits;
    assign y_alarm      = step_alarm;
    assign y_en_out     = y_en_in;
    assign y_dir_out    = y_dir_in;
    assign y_step_out   = y_step_in & all_limits;

    assign x_limit_com  = 1'b0;
    assign y_limit_com  = 1'b0;

    localparam WHEEL_STATUS_REG = 8'h90;
    localparam X_STATUS_REG     = 8'hA0;
    localparam Y_STATUS_REG     = 8'hB0;

    reg         x_status_intr_clear;
    wire        x_status_flip;
    wire        x_status_intr;
    wire [3:0]  x_status_out;
    signal      #(.MSB(3)) xsig
    (
        .clk    (clk),
        .rst    (rst),
        .en     (1'b1),
        .clr    (x_status_intr_clear),
        .in     (x_status),
        .out    (x_status_out),
        .flip   (x_status_flip),
        .intr   (x_status_intr)
    );

    reg         y_status_intr_clear;
    wire        y_status_flip;
    wire        y_status_intr;
    wire [3:0]  y_status_out;
    signal      #(.MSB(3)) ysig
    (
        .clk    (clk),
        .rst    (rst),
        .en     (1'b1),
        .clr    (y_status_intr_clear),
        .in     (y_status),
        .out    (y_status_out),
        .flip   (y_status_flip),
        .intr   (y_status_intr)
    );

    reg         w_status_intr_clear;
    wire        w_status_flip;
    wire        w_status_intr;
    wire [1:0]  w_status_out;
    signal      #(.MSB(1)) wsig
    (
        .clk    (clk),
        .rst    (rst),
        .en     (1'b1),
        .clr    (w_status_intr_clear),
        .in     ({wheel_a, wheel_b}),
        .out    (w_status_out),
        .flip   (w_status_flip),
        .intr   (w_status_intr)
    );

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

    assign out[0]   = x_status_out[0];
    assign out[1]   = x_status_out[1];
    assign out[2]   = x_status_out[3];
    assign out[3]   = x_status_intr;
    assign out[4]   = x_step_out;
    assign out[5]   = x_limit_db;
    assign out[6]   = y_step_out;
    assign out[7]   = y_limit_db;

    assign intr[0]  = w_status_intr;
    assign intr[1]  = x_status_intr;
    assign intr[2]  = y_status_intr;
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
        tx_valid            <= 1'b0;
        data_read           <= 1'b0;
        data_wr_valid       <= 1'b0;
        x_status_intr_clear <= 1'b1;
        y_status_intr_clear <= 1'b1;
        w_status_intr_clear <= 1'b1;

        if(spi_cs_rise) begin
            spi_byte_num    <= 8'h00;
            case (data_rd_addr)
                X_STATUS_REG        : x_status_intr_clear <= 1'b0;
                Y_STATUS_REG        : y_status_intr_clear <= 1'b0;
                WHEEL_STATUS_REG    : w_status_intr_clear <= 1'b0;
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

        if(x_status_flip) begin
            data_addr       <= X_STATUS_REG;
            data_wr_valid   <= 1'b1;
            data_in         <= {4'hF, x_status_out};
        end
        else if(y_status_flip) begin
            data_addr       <= Y_STATUS_REG;
            data_wr_valid   <= 1'b1;
            data_in         <= {4'hF, y_status_out};
        end
        else if(w_status_flip) begin
            data_addr       <= WHEEL_STATUS_REG;
            data_wr_valid   <= 1'b1;
            data_in         <= {6'h3F, w_status_out};
        end
    end

endmodule   