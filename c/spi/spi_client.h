#ifndef HOST_SPI_H
#define HOST_SPI_H

#include <pigpio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "spi_common.h"

typedef struct spi_ioc_transfer spiTr;
typedef struct
{
	int		cs;		// additional chip select (other than CS0 or CS1)
	int		fd;		// file descriptor
	spiTr	tr;		// spi transfer settings/data

	pthread_mutex_t* mutex;

} pincSPIclient_t;


static inline void spi_write_msg(pincSPIclient_t* client, void* tx_data, uint32_t bytes)
{
	// check bytes <= max spi transfer length

	client->tr.len		= bytes;
	client->tr.tx_buf	= (unsigned long)malloc(bytes);
	client->tr.rx_buf	= (unsigned long)malloc(bytes);

	memcpy((void*)client->tr.tx_buf, tx_data, bytes);
}

static inline void spi_read_msg(pincSPIclient_t* client, void* rx_data, uint32_t bytes)
{
	// check bytes <= max spi transfer length

	memcpy(rx_data, (void*)client->tr.rx_buf, bytes);

	free((void*)client->tr.tx_buf);
	free((void*)client->tr.rx_buf);
}

static inline void spi_lock(pincSPIclient_t* client)
{
	int err = pthread_mutex_lock(client->mutex);
	if(err != 0)
		printf("SPI Lock Error:\t%d\n", err);
}

static inline void spi_unlock(pincSPIclient_t* client)
{
	int err = pthread_mutex_unlock(client->mutex);
	if(err != 0)
		printf("SPI Unlock Error:\t%d\n", err);
}

static inline void spi_send(pincSPIclient_t* client)
{
	spi_lock(client);

	if(client->cs > 0)
		gpioWrite(client->cs, 0);
	
	ioctl(client->fd, SPI_IOC_MESSAGE(1), &client->tr);

	if(client->cs > 0)
		gpioWrite(client->cs, 1);

	spi_unlock(client);
}

#endif