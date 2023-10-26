#ifndef HOST_SPI_H
#define HOST_SPI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define MAX_SPI_TRANSFER_LENGTH	4096

typedef struct
{
	uint8_t*	tx;
	uint8_t*	rx;
	uint32_t	len;

} spiMsg;

static inline void spi_write_msg(spiMsg* msg, uint8_t* tx_data, uint32_t bytes)
{
	msg->tx = malloc(bytes);
	msg->rx = malloc(bytes);
	msg->len = bytes;
	memcpy(msg->tx, tx_data, bytes);
}

static inline void spi_read_msg(spiMsg* msg, uint8_t* rx_data, uint32_t bytes)
{
	memcpy(rx_data, msg->rx, bytes);
	free(msg->tx);
	free(msg->rx);
}

typedef struct spi_ioc_transfer spiTr;
typedef struct
{
	int		cs;
	int		fd;
	spiTr	tr;

	pthread_mutex_t* mutex;

} spiHost;

static void spi_init(	
	spiHost* host,
	char* filename,
	int cs,
	uint32_t speed_hz,
	uint16_t delay_us,
	uint8_t bpw,
	uint8_t cs_change,
	uint8_t mode,
	pthread_mutex_t* mutex)
{
	host->fd = open(filename, O_RDWR);
	ioctl(host->fd, SPI_IOC_WR_MODE, &mode);

	memset(&host->tr, 0, sizeof(spiTr));
	host->tr.speed_hz		= speed_hz;
	host->tr.delay_usecs	= delay_us;
	host->tr.bits_per_word	= bpw;
	host->tr.cs_change		= cs_change;

	if(cs > 0)
	{
		gpioSetMode(cs, PI_OUTPUT);
		gpioWrite(cs, 1);
	}
	host->cs = cs;

	host->mutex = mutex;
}

static inline void spi_lock(spiHost* host)
{
	int err = pthread_mutex_lock(host->mutex);
	if(err != 0)
		printf("SPI Lock Error:\t%d\n", err);
}
static inline void spi_unlock(spiHost* host)
{
	int err = pthread_mutex_unlock(host->mutex);
	if(err != 0)
		printf("SPI Unlock Error:\t%d\n", err);
}

static inline void spi_send(spiHost* host, spiMsg* msg)
{
	spi_lock(host);

	if(host->cs > 0)
		gpioWrite(host->cs, 0);

	host->tr.len	= msg->len;
	host->tr.tx_buf	= (unsigned long)msg->tx;
	host->tr.rx_buf	= (unsigned long)msg->rx;
	
	ioctl(host->fd, SPI_IOC_MESSAGE(1), &host->tr);

	if(host->cs > 0)
		gpioWrite(host->cs, 1);

	spi_unlock(host);
}

#endif