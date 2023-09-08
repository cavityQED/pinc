#include "spiDevice.h"

spiHost*	spiDevice::__spi_host = new spiHost("/dev/spidev0.0", SPI_MODE_3);

spiDevice::spiDevice(const QString& name, QWidget* parent) : pincDevice(name, parent)
{

}

spiGroup::spiGroup(const QString& name, QWidget* parent) : spiDevice(name, parent)
{

}

void spiGroup::addDevice(spiDevice* dev)
{
	m_devices.push_back(dev);
}

void spiGroup::trigger()
{
	for(auto dev : m_devices)
	{
		dev->trigger();
		__spi_host->wait_for_sem();
	}
}

void spiGroup::reload()
{
	for(auto dev : m_devices)
		dev->reload();
}