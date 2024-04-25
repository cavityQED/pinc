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

#define PICO_STEP_PIN		16
#define PICO_DIR_PIN		17
#define PICO_ENA_PIN		28

typedef struct repeating_timer picoTimer_t;
struct stepper
{
	bool		dir;		// direction
	int			p_step;		// step pin
	int			p_ena;		// enable pin
	int			p_dir;		// direction pin
	int			p_motion;	// active low if (status & MOTION)
	int			pos;		// position (steps)
	int			delay;		// dleay between steps (us)

	uint8_t		axis;
	uint8_t		status;
	uint16_t	spmm;	//steps per mm

	uint32_t	accel;
	uint32_t	jog_speed;
	uint32_t	min_speed;
	uint32_t	max_speed;

	pincStepperMove_t	move;
	picoTimer_t*		timer;
	alarm_pool_t*		alarmPool;
	semaphore_t			sem;
};

static struct stepper	motor;

static inline void set_speed(struct stepper* s, float mmps)
	{s->delay = 1000000 / mmps / s->spmm;}

static inline float mm_pos(const struct stepper* s)
	{return (float)s->pos/s->spmm;}

static inline bool check_status(struct stepper* s, uint8_t mask)
	{return s->status & mask;}

void stepper_init		(struct stepper* s);
void set_dir			(struct stepper* s);
void step				(struct stepper* s);
void stepper_move		(struct stepper* s);
void stepper_msg_handle	(struct stepper* s, uint8_t* msg);
void stepper_line_move	(struct stepper* s);

bool motor_timer_cb	(struct repeating_timer* timer);
void callback		(uint gpio, uint32_t event_mask);

#endif