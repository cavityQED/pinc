#ifndef STEPPER_H
#define STEPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"

#include "c/stepper/stepper_common.h"
#include "spi/spi_client.h"

#define PICO_PIN_MOTION			10
#define PICO_PIN_SYNC_READY		11
#define PICO_PIN_MOVE_READY		12
#define PICO_PIN_SPI_MSG_READY	13

#define PICO_PIN_SYNC_SIGNAL	14
#define PICO_PIN_SPI_MSG_REQ	15

#define PICO_PIN_ALARM			18
#define PICO_PIN_STEP			19
#define PICO_PIN_DIR			20
#define PICO_PIN_ENABLE			21

typedef struct repeating_timer picoTimer_t;
struct stepper
{
	bool		dir;			// direction
	int			p_step;			// step pin
	int			p_ena;			// enable pin
	int			p_dir;			// direction pin
	int			p_motion;		// active low if (status & MOTION)
	int			p_sync_ready;	
	int			p_move_ready;
	int			pos;			// position (steps)

	bool		homed;
	bool		alarm;

	uint8_t		status;

	pincStepperMove_t		move;
	pincStepperConfig_t		config;
	alarm_pool_t*			alarmPool;
	alarm_id_t				alarm_id;
	queue_t					msgQueue;

	semaphore_t				sem;
};

static struct stepper	motor;

static inline void set_speed(struct stepper* s, float mmps)
	{s->move.delay = 1000000 / mmps / s->config.spmm;}

static inline float mm_pos(const struct stepper* s)
	{return (float)s->pos/s->config.spmm;}

static inline bool check_status(struct stepper* s, uint8_t mask)
	{return s->status & mask;}

void stepper_init		(struct stepper* s);
void set_dir			(struct stepper* s);
void step				(struct stepper* s);
void stepper_move		(struct stepper* s);
void stepper_home		(struct stepper* s);
void stepper_msg_handle	(struct stepper* s, uint8_t* msg);

#endif