#ifndef PINC_H
#define PINC_H

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"

#include "spi/spi_client.h"
#include "stepper.h"
#include "c/host_common.h"

static uint8_t spi_in_buf[SPI_TRANSFER_LENGTH];
static uint8_t spi_out_buf[SPI_TRANSFER_LENGTH];

static picoTimer_t			step_timer;
static struct spi_client	spi;
static struct stepper		motor;
static queue_t				spiQueue;

#endif