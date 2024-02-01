#ifndef SIGNAL_H
#define SIGNAL_H

#include "pigpio.h"

#include <semaphore.h>

#include "algorithm/bst.h"
#include "host_common.h"



typedef struct
{
	uint8_t pre;
	uint8_t cur;
	uint8_t del;
	uint8_t low;
	uint8_t high;

	uint16_t mask;

} signal_t;

typedef struct request_t request_t;
typedef void (*req_func)(request_t*);
struct request_t
{
	bool			ex;		// exclusive; uses mutex if true
	bool			level;
	uint8_t			bit;
	uint8_t			sem_max;
	uint16_t		mask;
	sem_t			sem;
	req_func		func;
	pthread_mutex_t mutex;
};


static inline void request_init(request_t* req, bool level, uint8_t bit)
{
	req->level		= level;
	req->bit		= bit;
	req->mask		= 0xFF00 | bit;

	pthread_mutex_init(&req->mutex, NULL);
	sem_init(&req->sem, 0, 0);
}

static inline void request_unclaim(request_t* req)
{
	pthread_mutex_unlock(&req->mutex);
	printf("\tRequest Unclaimed:\t0x%4X\n", req->mask);
}

static inline void request_claim(request_t* req)
{
	printf("\tRequest Claiming:\t0x%4X\n", req->mask);
	pthread_mutex_lock(&req->mutex);
	printf("\tRequest Claimed:\t0x%4X\n", req->mask);
}

static inline void request_lock(request_t* req)
{
	printf("\tRequest Waiting:\t0x%4X\n", req->mask);
	sem_wait(&req->sem);
}

static inline void request_unlock(request_t* req)
{
	sem_post(&req->sem);
	printf("\tRequest Unlocked:\t0x%4X\n", req->mask);
}

static inline bool request_check(request_t* req, signal_t* sig)
{
	int err = pthread_mutex_trylock(&req->mutex);
	if(err == 0) // wasn't locked
	{
		request_unclaim(req);
		return false;
	}

	uint8_t mask = (req->level)? sig->cur : ~sig->cur;
	return req->bit & mask;
}

static inline bool request_check_over(request_t* req, signal_t* sig)
{
	return req->bit == ((req->level)? sig->low : sig->high);
}










typedef struct
{
	int				pin;
	bool			level;
	bool			fired;

	uint8_t			sem_max;
	sem_t			sem;
	request_t*		req;
	pthread_mutex_t mutex;

} pin_request_t;


static inline void pin_request_init(pin_request_t* pin_req, request_t* req, int pin, bool level, uint8_t max)
{
	pin_req->fired	= false;
	pin_req->req	= req;

	gpioSetMode(pin, PI_OUTPUT);
	gpioWrite(pin, !level);
	pin_req->pin	= pin;
	pin_req->level	= level;

	pthread_mutex_init(&pin_req->mutex, NULL);
	pthread_mutex_lock(&pin_req->mutex);

	sem_init(&pin_req->sem, 0, 0);
	pin_req->sem_max = max;
}

static inline void pin_request_trigger(pin_request_t* pin_req)
{
	int sval;
	sem_getvalue(&pin_req->sem, &sval);
	if(sval > pin_req->sem_max)
	{
		sem_wait(&pin_req->sem);
		pin_request_trigger(pin_req);
	}
	else
	{
		gpioWrite(pin_req->pin, pin_req->level);
		pin_req->fired = true;
	}
	printf("\tPin Request Triggered on Pin %d\n", pin_req->pin);
}

static inline void pin_request_reset(pin_request_t* pin_req)
{
	gpioWrite(pin_req->pin, !pin_req->level);
	printf("\tPin Request Reset on Pin %d\n", pin_req->pin);
	if(pin_req->req->ex)
		request_unclaim(pin_req->req);
}

static inline void pin_request_ack(pin_request_t* pin_req)
{
	sem_post(&pin_req->sem);
	printf("\tRequest Unlocked on Pin %d\n", pin_req->pin);
}

static inline void pin_request_wait(pin_request_t* pin_req)
{
	printf("\tRequest Waiting on Pin %d\n", pin_req->pin);
	sem_wait(&pin_req->sem);
}

static inline void pin_request_blocking(pin_request_t* pin_req)
{
	if(pin_req->req->ex)
		request_claim(pin_req->req);
	pin_request_trigger(pin_req);
	request_lock(pin_req->req);
}





















typedef struct
{


} pin_signal_t;













#endif