#ifndef FPGA_SIGNAL_H
#define FPGA_SIGNAL_H

#include "spi/host_spi.h"
#include "signal.h"

typedef void (*fpga_func)(int, int, uint32_t, void*);

typedef struct
{
	int			intr_pin;
	void*		dev;
	spiHost*	spi_host;
	signal_t	sig;

} fpga_signal;

static void fpga_get_signal(void* dev)
{
	static uint8_t	tx[2];
	static uint8_t	rx[2];
	static spiMsg	msg = {tx, rx, 2};

	fpga_signal*	fpga	= (fpga_signal*)dev;
	signal_t* 		sig		= &(fpga->sig);

	sig->pre = sig->cur;

	spi_send(fpga->spi_host, &msg);

	sig->cur = rx[1];
	sig->del = sig->pre ^ sig->cur;
	sig->low = sig->del & (~sig->cur);
	sig->high = sig->del & sig->cur;
	sig->mask = (sig->high << 8) | sig->low;

	printf("FPGA Signal Recieved:\t0x%2X\n", rx[1]);
}

static void fpga_sig_init(fpga_signal* fpga, int pin, void* dev, spiHost* spi, fpga_func func)
{
	gpioSetMode(pin, PI_INPUT);
	gpioSetPullUpDown(pin, PI_PUD_UP);
	fpga->intr_pin	= pin;
	fpga->dev		= dev;
	fpga->spi_host	= spi;

	if(dev)
		gpioSetISRFuncEx(pin, FALLING_EDGE, 0, func, dev);
	else
		gpioSetISRFuncEx(pin, FALLING_EDGE, 0, func, fpga);

	fpga->sig.cur = 0xFF;
	fpga->sig.pre = 0xFF;
}




#endif