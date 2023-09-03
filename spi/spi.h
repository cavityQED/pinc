#ifndef SPI_H
#define SPI_H

#include "pigpio.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <semaphore.h>
#include <mutex>

#define SPI_READY_PIN	25

#define	SPI_WAITING		0x01
#define SPI_IDLE		0x02
#define SPI_SENDING		0x04

#define SPI_ERROR		0x40

namespace SPI
{

	static 	int		fd;
	static	sem_t*	sem;
	static	uint8_t	status;
	static	bool	initialized;

	struct msg
	{
		spi_ioc_transfer		transfer;
		
		std::vector<uint8_t>	sendbuf;
		std::vector<uint8_t>	recvbuf;

		msg(uint8_t maxlength = 64)
		{
			std::memset(&transfer, 0, sizeof(transfer));

			sendbuf.resize(maxlength);
			recvbuf.resize(maxlength);

			transfer.tx_buf = sendbuf.data();
			transfer.rx_buf = recvbuf.data();

			transfer.len			= maxlength;	//Max transfer length in bytes
			transfer.speed_hz		= 1000000;		//1Mhz clock speed
			transfer.delay_usecs	= 100;			//Delay after last clock pulse
			transfer.bits_per_word	= 8;
		}
	};
}

#endif