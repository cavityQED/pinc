#ifndef HOST_STEPPER_H
#define HOST_STEPPER_H

#include "request.h"
#include "device/pcf8574.h"
#include "spi/host_spi.h"

#include "stepper_common.h"

typedef struct
{
	uint8_t		axis;
	uint8_t		status;
	uint16_t 	spmm;

	int			step_pos;

} StepperData;

typedef struct
{
	uint8_t			i2c_addr;
	PCF				signals;	

	int				pin_isr;	// signal interrupt pin, active low
	Request			req_spi;	// request to send an spi message	
	Request			req_sync;	// request for sync move setup
	spiMsg			msg;
	spiHost*		spi_host;	
	StepperData		data;
	pthread_mutex_t mutex;

} Stepper;

void stepper_init(Stepper* s);
void stepper_move(Stepper* s, StepperMsg* msg);
void stepper_spi_send(Stepper* s);

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev);



#endif