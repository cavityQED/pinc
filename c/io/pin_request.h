#ifndef PIN_REQUEST_H
#define PIN_REQUEST_H

#include "pigpio.h"

#include <semaphore.h>

typedef struct
{
	uint8_t pre;
	uint8_t cur;
	uint8_t del;
	uint8_t low;
	uint8_t high;

	uint16_t mask;

} signal_t;

static inline void signal_update(signal_t* sig_out, uint8_t sig_in)
{
	sig_out->pre 	= sig_out->cur;
	sig_out->del 	= sig_out->pre ^ sig_in;
	sig_out->low 	= sig_out->del & ~sig_in;
	sig_out->high	= sig_out->del & sig_in;
	sig_out->cur 	= sig_in;
}

static inline void signal_print(signal_t* sig)
{
	printf("Signal:\n");
	printf("\t[Pre, Cur, Del]:\t[%2X, %2X, %2X]\n", sig->pre, sig->cur, sig->del);
	printf("\t[High, Low]:\t\t[%2X, %2X]\n", sig->high, sig->low);
	printf("\tMask:\t\t\t%4X\n\n", sig->mask);
}





typedef struct
{
	int		pin;
	bool	level;
	bool	fired;

	sem_t	sem;

	pthread_mutex_t*	mutex;

} pin_request_t;


static inline void pin_request_lock(pin_request_t* pin_req)
{
	pthread_mutex_lock(pin_req->mutex);
}

static inline void pin_request_unlock(pin_request_t* pin_req)
{
	pthread_mutex_unlock(pin_req->mutex);
}

static inline void pin_request_wait(pin_request_t* pin_req)
{
	sem_wait(&pin_req->sem);
}

static inline void pin_request_post(pin_request_t* pin_req)
{
	sem_post(&pin_req->sem);
}

static inline void pin_request_init(pin_request_t* pin_req, int pin, bool level, pthread_mutex_t* mutex)
{
	pin_req->fired	= false;

	gpioSetMode(pin, PI_OUTPUT);
	gpioWrite(pin, !level);
	pin_req->pin	= pin;
	pin_req->level	= level;
	pin_req->mutex	= mutex;

	sem_init(&pin_req->sem, 0, 0);
}

static inline void pin_request_trigger(pin_request_t* pin_req)
{
	pin_request_lock(pin_req);

	if(!pin_req->fired)
	{
		gpioWrite(pin_req->pin, pin_req->level);
		pin_req->fired = true;
	}
}

static inline void pin_request_reset(pin_request_t* pin_req)
{
	if(pin_req->fired)
	{
		gpioWrite(pin_req->pin, !pin_req->level);
		pin_req->fired = false;
	}

	pin_request_unlock(pin_req);
}

static inline void pin_request_blocking(pin_request_t* pin_req)
{
	pin_request_trigger(pin_req);
	pin_request_wait(pin_req);
}


#endif