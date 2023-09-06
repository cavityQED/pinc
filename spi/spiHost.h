#ifndef SPI_HOST_H
#define SPI_HOST_H

#include "pigpio.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <semaphore.h>
#include <mutex>

#include "device/spiDevice.h"
#include "spiMsg.h"

#define	READY_PIN	25

class spiDevice;

class spiHost
{

public:

	spiHost(const char* pathname, const uint8_t spiMode);
	~spiHost() {sem_close(m_spi_sem);}

	void send(spiDevice* dev, spiMsg* msg);
	void groupSend(const std::vector<spiDevice*>& devs, spiMsg* msg);

	std::mutex& lock() {return m_lock;}

	static void spi_sem_post_alertFunc(int gpio, int level, uint32_t tick)
	{
		if(m_spi_waiting && level)
		{
			sem_post(m_spi_sem);
			m_spi_waiting = false;
		//	std::cout << "\tSem Posted on Rising Edge\n";
		}

		else if(!m_spi_waiting && level)
		{
			std::cout << "\tFalse Trigger\n";
		}

		else if(!level)
		{
		//	std::cout << "\tReady Pin Reset\n";
		}

		else
			return;
	}

protected:

	int				m_file_descriptor;
	static sem_t*	m_spi_sem;
	static bool		m_spi_waiting;
	std::mutex		m_lock;
};

#endif