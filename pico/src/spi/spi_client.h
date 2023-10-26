#ifndef SPI_CLIENT_H
#define SPI_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/structs/bus_ctrl.h"

#include "spi.pio.h"

#define PIO_SPI_DEFAULT_IN_PIN			0
#define PIO_SPI_DEFAULT_OUT_PIN			3
#define PIO_SPI_BITS_PER_WORD			8

struct spi_client
{
	PIO			pio;
	uint32_t	sm;			// state machine number
	uint32_t	offset;		// pio program offset
	uint32_t	clk_div;	// clock divider

	uint32_t	in_pin;
	uint32_t	cs_pin;
	uint32_t	clk_pin;
	uint32_t	out_pin;
	uint32_t	tr_pin;		// transfer ready pin

	uint32_t	bits;		// bits per word
	uint32_t	words;		// words per transfer
	uint8_t*	tx_buf;
	uint8_t*	rx_buf;
	
	uint		tx_chan;
	uint		rx_chan;


	pio_sm_config		config;
	dma_channel_config	tx_config;
	dma_channel_config	rx_config;
};

void pio_spi_client_init(struct spi_client* spi);

static inline void pio_spi_client_enable(struct spi_client* spi)
{
	pio_sm_set_pindirs_with_mask(	spi->pio,
									spi->sm,
									1u << spi->out_pin,
									1u << spi->out_pin);

	pio_gpio_init		(spi->pio, spi->out_pin);
	pio_sm_exec			(spi->pio, spi->sm, pio_encode_set(pio_x, spi->words - 1));	// set x register
	pio_sm_exec			(spi->pio, spi->sm, pio_encode_jmp(spi->offset));			// reset PC
	pio_sm_set_enabled	(spi->pio, spi->sm, true);

	gpio_put(spi->tr_pin, 0);
}

static inline void pio_spi_client_disable(struct spi_client* spi)
{
	pio_sm_set_enabled	(spi->pio, spi->sm, false);
	pio_sm_clear_fifos	(spi->pio, spi->sm);
	pio_sm_restart		(spi->pio, spi->sm);

	gpio_set_function(spi->out_pin, GPIO_FUNC_NULL);

	dma_channel_abort(spi->tx_chan);
	dma_channel_abort(spi->rx_chan);
	
	gpio_put(spi->tr_pin, 1);
}

static inline int get_dma_transfer_size(struct spi_client* spi)
{
	switch(spi->bits)
	{
		case 32:
			return DMA_SIZE_32;
		case 16:
			return DMA_SIZE_16;
		case 8:
		default:
			return DMA_SIZE_8;
	}
}

static inline void pio_spi_dma_start(struct spi_client* spi)
{
	dma_channel_configure(	spi->tx_chan,
							&spi->tx_config,
							&spi->pio->txf[spi->sm],
							spi->tx_buf,
							spi->words,
							true);

	dma_channel_configure(	spi->rx_chan,
							&spi->rx_config,
							spi->rx_buf,
							&spi->pio->rxf[spi->sm],
							spi->words,
							true);
}

static inline void pio_spi_client_transfer(struct spi_client* spi)
{
	pio_spi_client_disable(spi);
	pio_spi_dma_start(spi);
	pio_spi_client_enable(spi);
}

static inline void pio_spi_client_transfer_blocking(struct spi_client* spi)
{
	pio_spi_client_transfer(spi);

	dma_channel_wait_for_finish_blocking(spi->tx_chan);
	dma_channel_wait_for_finish_blocking(spi->rx_chan);
}

#endif