#include "spiHost.h"

sem_t*	spiHost::m_spi_sem		= sem_open("temp", O_CREAT, S_IRWXO, 0);
bool	spiHost::m_spi_waiting	= false;

spiHost::spiHost(const char* pathname, const uint8_t spiMode)
{
	m_file_descriptor = open(pathname, O_RDWR);

	if(m_file_descriptor < 0)
	{
		std::cout << "Error Opening SPI Device " << pathname << "\n";
		std::cout << '\t' << std::strerror(errno) << '\n';
		return;
	}

	else
	{
		std::cout << "SPI Open Successful\n";
		std::cout << "\tDevice File:\t" << pathname << '\n';
		std::cout << "\tFile Descriptor:\t" << m_file_descriptor << '\n';
	}

	//set the spi transfer mode
	ioctl(m_file_descriptor, SPI_IOC_WR_MODE, &spiMode);

	gpioInitialise();
	gpioSetMode(READY_PIN, PI_INPUT);
	gpioSetAlertFunc(READY_PIN, spi_sem_post_alertFunc);

	m_spi_sem = sem_open("spi_sem", O_CREAT, S_IRWXO, 0);
}

void spiHost::send(spiMsg* msg)
{
	ioctl(m_file_descriptor, SPI_IOC_MESSAGE(1), msg->tr_ptr());
}

void spiHost::send(spiDevice* dev, spiMsg* msg)
{
	int val;
	sem_getvalue(m_spi_sem, &val);

	if(!val)
	{
		m_spi_waiting = true;
		dev->trigger();
		sem_wait(m_spi_sem);
		dev->reload();
		msg->clear_recv_buf();
		ioctl(m_file_descriptor, SPI_IOC_MESSAGE(1), msg->tr_ptr());
	}

	else
	{
		sem_wait(m_spi_sem);
		send(dev, msg);
	}
}

void spiHost::groupSend(const std::vector<spiDevice*>& devs, spiMsg* msg)
{
	for(auto d : devs)
	{
		m_spi_waiting = true;
		d->trigger();
		sem_wait(m_spi_sem);
		d->reload();
	}

	ioctl(m_file_descriptor, SPI_IOC_MESSAGE(1), msg->tr_ptr());
}