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

protected:

	static spiHost*	__spi_host;
};

#endif