#ifndef PINC_H
#define PINC_H

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"

#include "spi/spi_client.h"
#include "stepper.h"
#include "c/host_common.h"

#define PIN_SYNC_READY			18
#define PIN_MOTION				10
#define PIN_DIRECTION			11
#define PIN_MOVE_READY			12
#define PIN_SPI_MSG_READY		13

#define PIN_SYNC_SIGNAL			26
#define PIN_SPI_MSG_REQUEST		15

static uint8_t spi_in_buf[SPI_TRANSFER_LENGTH];
static uint8_t spi_out_buf[SPI_TRANSFER_LENGTH];

static picoTimer_t			step_timer;
static struct spi_client	spi;
static struct stepper		motor;
static queue_t				spiQueue;
static queue_t				msgQueue;

#endif