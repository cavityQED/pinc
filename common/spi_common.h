#ifndef SPI_COMMON_H
#define SPI_COMMON_H

#include <iostream>
#include <mutex>

namespace SPI
{
	enum spi_status_t
	{
		SENDING,
		IDLE,
		RECEIVING
	};

}// SPI namespace


#endif