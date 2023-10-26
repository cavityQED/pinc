#include "host_stepper.h"

void stepper_init(Stepper* s)
{
	request_init(&s->req_spi);
	request_init(&s->req_sync);

	pcf_init(&s->signals, s->i2c_addr, s->pin_isr);
	gpioSetISRFuncEx(s->pin_isr, FALLING_EDGE, 0, stepper_status_isr, s);

	pthread_mutex_init(&s->mutex, NULL);
}

void stepper_spi_send(Stepper* s)
{
	request_blocking(&s->req_spi);

	spi_send(s->spi_host, &s->msg);

	request_reset(&s->req_spi);
}

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev)
{
	Stepper* stepper = (Stepper*)dev;

	pcf_get_signals(&stepper->signals);

	uint8_t cur_mask = stepper->signals.low;

	if(request_check(&stepper->req_spi, cur_mask))
		request_ready(&stepper->req_spi);	
	else if(request_check(&stepper->req_sync, cur_mask))
		request_ready(&stepper->req_sync);
}