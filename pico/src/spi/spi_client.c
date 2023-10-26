#include "spi_client.h"

static void pio_spi_pins_init(struct spi_client* spi)
{
	spi->clk_pin = spi->in_pin + 1;
	spi->cs_pin = spi->in_pin + 2;

	pio_sm_set_pindirs_with_mask(	spi->pio,
									spi->sm,
									0,
									7u << spi->in_pin);

	pio_gpio_init(spi->pio, spi->in_pin);
	pio_gpio_init(spi->pio, spi->cs_pin);
	pio_gpio_init(spi->pio, spi->clk_pin);

	gpio_init(spi->tr_pin);
	gpio_set_dir(spi->tr_pin, GPIO_OUT);
	gpio_put(spi->tr_pin, 1);
}

static void pio_spi_sm_config(struct spi_client* spi)
{
	pio_sm_config pioc = spi_mode0_program_get_default_config(spi->offset);

	sm_config_set_in_pins	(&pioc, spi->in_pin);
	sm_config_set_out_pins	(&pioc, spi->out_pin, 1);
	sm_config_set_out_shift	(&pioc, false, true, spi->bits);	// shift osr left, autopull
	sm_config_set_in_shift	(&pioc, false, true, spi->bits);	// shift isr left, autopush
	sm_config_set_clkdiv	(&pioc, spi->clk_div);

	spi->config = pioc;
}

static void pio_spi_dma_channel_config(struct spi_client* spi)
{
	spi->tx_chan = dma_claim_unused_channel(true);
	spi->rx_chan = dma_claim_unused_channel(true);

	dma_channel_config txc = dma_channel_get_default_config(spi->tx_chan);
	dma_channel_config rxc = dma_channel_get_default_config(spi->rx_chan);

	channel_config_set_transfer_data_size(&txc, get_dma_transfer_size(spi));
	channel_config_set_transfer_data_size(&rxc, get_dma_transfer_size(spi));

	channel_config_set_write_increment(&txc, false);
	channel_config_set_write_increment(&rxc, true);

	channel_config_set_read_increment(&txc, true);
	channel_config_set_read_increment(&rxc, false);

	channel_config_set_dreq(&txc, pio_get_dreq(spi->pio, spi->sm, true));
	channel_config_set_dreq(&rxc, pio_get_dreq(spi->pio, spi->sm, false));

	spi->tx_config = txc;
	spi->rx_config = rxc;
}

void pio_spi_client_init(struct spi_client* spi)
{
	spi->offset = pio_add_program(spi->pio, &spi_mode0_program);

	pio_spi_pins_init(spi);
	pio_spi_sm_config(spi);
	pio_spi_dma_channel_config(spi);

	hw_set_bits(&spi->pio->input_sync_bypass, 7u << spi->in_pin);

	pio_sm_init(	spi->pio,
					spi->sm,
					spi->offset,
					&spi->config);

	bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
	pio_spi_client_disable(spi);
}