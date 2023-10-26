#ifndef REQUEST_H
#define REQUEST_H

#include "pigpio.h"

#include <stdint.h>
#include <pthread.h>

typedef struct
{
	int pin;
	int act;
	uint8_t mask;
	pthread_mutex_t mutex;

} Request;

static inline void request_reset(Request* req)
{
	int on = (req->act == 1)? 0 : 1;
	gpioWrite(req->pin, on);
}

static inline void request_trigger(Request* req)
{
	gpioWrite(req->pin, req->act);
}

static inline void request_init(Request* req)
{
	gpioSetMode(req->pin, PI_OUTPUT);
	request_reset(req);

	pthread_mutex_init(&req->mutex, NULL);
	pthread_mutex_lock(&req->mutex);
}

static inline void request_wait(Request* req)
{
	pthread_mutex_lock(&req->mutex);
}

static inline void request_ready(Request* req)
{
	pthread_mutex_unlock(&req->mutex);
}

static inline void request_try(Request* req)
{
	request_trigger(req);

	pthread_mutex_trylock(&req->mutex);
}

static inline void request_blocking(Request* req)
{
	request_trigger(req);
	request_wait(req);
}

static inline uint8_t request_check(Request* req, uint8_t check)
{
	return check & req->mask;
}


#endif