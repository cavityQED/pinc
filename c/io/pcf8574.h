#ifndef PCF8574_H
#define PCF8574_H

#include "pigpio.h"

#include <string.h>
#include <stdio.h>

#include "signal.h"

typedef void (*pcf_func)(int, int, uint32_t, void*);

typedef struct pcf8574_t
{
	int			pin;
	int			handle;	// i2c pigpio device handle
	uint8_t		addr;	// i2c address
	
	signal_t	sig;

	void*		dev;

} PCF;

static inline void pcf_init(PCF* pcf, uint8_t addr, int pin, void* dev, pcf_func func)
{
	gpioSetMode(pin, PI_INPUT);
	gpioSetPullUpDown(pin, PI_PUD_UP);
	pcf->pin	= pin;
	pcf->dev	= dev;
	pcf->addr	= addr;
	pcf->handle	= i2cOpen(1, addr, 0);

	if(pcf->dev && func)
		gpioSetISRFuncEx(pin, FALLING_EDGE, 0, func, dev);
	else if(func)
		gpioSetISRFuncEx(pin, FALLING_EDGE, 0, func, pcf);

	i2cWriteByte(pcf->handle, 0xFF);
	pcf->sig.cur = i2cReadByte(pcf->handle);
	pcf->sig.pre = pcf->sig.cur;
	pcf->sig.low = 0;
}

static void pcf_get_signal(void* dev)
{
	PCF* pcf = (PCF*)dev;

printf("\nPCF (addr, pin):\t(%Xh, %d)\n", pcf->addr, pcf->pin);
	pcf->sig.pre = pcf->sig.cur;
	pcf->sig.cur = i2cReadByte(pcf->handle);
	pcf->sig.del = pcf->sig.pre ^ pcf->sig.cur;
	pcf->sig.low = pcf->sig.del & (~pcf->sig.cur);
	pcf->sig.high = pcf->sig.del & pcf->sig.cur;
	pcf->sig.mask = (pcf->sig.high << 8) | pcf->sig.low;
printf("\tCurrent:\t%Xh\n", pcf->sig.cur);
printf("\tLow Edge:\t%Xh\n", pcf->sig.low);
printf("\tHigh Edge:\t%Xh\n", pcf->sig.high);
}


#endif