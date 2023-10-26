#ifndef HOST_SPI_H
#define HOST_SPI_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <pthread.h>

typedef struct spi_ioc_transfer spiTr;

static pthread_mutex_t spi_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
	uint8_t*	tx;
	uint8_t*	rx;
	uint32_t	len;

} spiMsg;

typedef struct
{
	int		fd;
	spiTr	tr;

} spiHost;

static void spi_init(	
	spiHost* host,
	char* filename,
	uint32_t speed_hz,
	uint16_t delay_us,
	uint8_t bpw,
	uint8_t cs_change,
	uint8_t mode)
{
	host->fd = open(filename, O_RDWR);
	ioctl(host->fd, SPI_IOC_WR_MODE, &mode);

	memset(&host->tr, 0, sizeof(spiTr));
	host->tr.speed_hz		= speed_hz;
	host->tr.delay_usecs	= delay_us;
	host->tr.bits_per_word	= bpw;
	host->tr.cs_change		= cs_change;
}

static inline void spi_send(spiHost* host, spiMsg* msg)
{
	pthread_mutex_lock(&spi_mutex);

	host->tr.len	= msg->len;
	host->tr.tx_buf	= (unsigned long)msg->tx;
	host->tr.rx_buf	= (unsigned long)msg->rx;

	ioctl(host->fd, SPI_IOC_MESSAGE(1), &host->tr);

	pthread_mutex_unlock(&spi_mutex);
}

#endif