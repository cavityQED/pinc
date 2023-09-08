#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include "device.h"
#include "spi/spiHost.h"

class spiHost;

class spiDevice : public pincDevice
{
	Q_OBJECT

public:

	spiDevice(const QString& name = "", QWidget* parent = nullptr);
	~spiDevice() {}

	virtual void trigger()	= 0;
	virtual void reload()	= 0;

	spiHost* host() {return __spi_host;}

protected:

	static spiHost*	__spi_host;
};


class spiGroup : public spiDevice
{
	Q_OBJECT

public:

	spiGroup(const QString& name = "", QWidget* parent = nullptr);
	~spiGroup() {}

	virtual void trigger() override;
	virtual void reload() override;

	void addDevice(spiDevice* dev);

protected:

	std::vector<spiDevice*> m_devices;
};

#endif