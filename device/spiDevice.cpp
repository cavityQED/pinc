#include "spiDevice.h"

spiHost*	spiDevice::__spi_host = new spiHost("/dev/spidev0.0", SPI_MODE_3);

spiDevice::spiDevice(const QString& name, QWidget* parent) : pincDevice(name, parent)
{

}