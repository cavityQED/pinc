#ifndef REQUEST_H
#define REQUEST_H

#include "pigpio.h"

#include <stdint.h>
#include <pthread.h>

#include "host_common.h"

typedef struct
{
	int 				pin;
	bool 				on;
	bool 				requested;
	bool				level;
	uint8_t 			bit;
	pthread_mutex_t		mutex;
	pthread_mutex_t*	shared_mutex;

} Request;

static inline void request_claim(Request* req)
{
	pthread_mutex_lock(req->shared_mutex);
}

static inline void request_unclaim(Request* req)
{
	pthread_mutex_unlock(req->shared_mutex);
}

static inline void request_reset(Request* req)
{
	request_unclaim(req);
	gpioWrite(req->pin, !req->on);
	req->requested = false;
}

static inline void request_init(Request* req)
{
	gpioSetMode(req->pin, PI_OUTPUT);
	request_reset(req);

	req->requested = false;

	pthread_mutex_init(&req->mutex, NULL);
	pthread_mutex_lock(&req->mutex);
}

static inline void request_trigger(Request* req)
{
	gpioWrite(req->pin, req->on);
	req->requested = true;
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
	request_claim(req);

	request_trigger(req);

	pthread_mutex_trylock(&req->mutex);
}

static inline void request_blocking(Request* req)
{
	request_claim(req);
	request_trigger(req);
	request_wait(req);
}


#endif