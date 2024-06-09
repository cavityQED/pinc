#include "stepper.h"

void stepper_init(struct stepper* s)
{
	s->alarm = false;

	uint mask = (1 << s->p_step)		| 
				(1 << s->p_dir)			| 
				(1 << s->p_ena)			| 
				(1 << s->p_motion)		| 
				(1 << s->p_sync_ready)	|
				(1 << s->p_move_ready);

	uint msg_spinlock = spin_lock_claim_unused(true);
	queue_init_with_spinlock(	&s->msgQueue,
								MAX_SPI_TRANSFER_LENGTH,
								2,
								msg_spinlock);				

	gpio_init_mask(mask);
	gpio_set_dir_out_masked(mask);
	gpio_put(s->p_ena, 0);
	gpio_put(s->p_motion, 1);
	gpio_put(s->p_sync_ready, 1);
	gpio_put(s->p_move_ready, 1);

	sem_init(&s->sem, 0, 1);
}

void set_dir(struct stepper* s)
{
	gpio_put(s->p_dir, s->dir);
	if(s->dir)
		s->status |= PICO_STATUS_DIRECTION;
	else
		s->status &= ~PICO_STATUS_DIRECTION;
}

void step(struct stepper* s)
{
	gpio_put(s->p_step, 1);
	int add = (s->dir)? 1 : -1;
	s->pos += add;
	sleep_us(8);
	gpio_put(s->p_step, 0);
}

void stop(struct stepper* s)
{
	s->status	&= ~(PICO_STATUS_IN_MOTION | PICO_STATUS_SYNC_READY);
	s->status	|= PICO_STATUS_MOVE_READY;

	alarm_pool_cancel_alarm(s->alarmPool, s->alarm_id);

	if((s->move.mode & HOME_MOVE) && s->alarm)
	{
		printf("Limit Switch Tripped\n");
		s->homed = true;
		s->pos = -2*s->config.jog_steps;

		memset(&s->move, 0, sizeof(pincStepperMove_t));
		uint8_t msg[MAX_SPI_TRANSFER_LENGTH];

		s->move.mode		= JOG_MOVE;
		s->move.step_dir	= 1;
		s->move.steps		= 2*s->config.jog_steps;
		s->move.v0_sps		= s->config.min_speed;
		s->move.vf_sps		= s->config.jog_speed;
		s->move.accel		= 400000;

		msg[0] = STEPPER_CMD_MOVE;
		memcpy(&msg[1], &s->move, sizeof(pincStepperMove_t));

		queue_try_add(&s->msgQueue, msg);
	}
	else
		gpio_set_mask((1 << s->p_motion) | (1 << s->p_sync_ready) | (1 << s->p_move_ready));

	s->alarm = false;
}

int64_t sync_alarm_callback(alarm_id_t id, void* user_data)
{
	struct stepper* s = (struct stepper*)user_data;

	if(s->move.mode & LINE_MOVE)
	{
		line_step_2d(&s->move);
		s->move.steps = s->move.steps - 1;
		if(s->move.accel)
			stepper_accel(&s->move);
	}
	else if(s->move.mode & CURVE_MOVE)
		curve_step_2d(&s->move);

	if(s->move.stop)
	{
		stop(s);
		return 0;
	}
	else if(s->config.axis == s->move.step_axis)
	{
		s->dir = s->move.step_dir;
		set_dir(s);
		step(s);
	}

	return -(int64_t)s->move.delay;	
}

int64_t jog_alarm_callback(alarm_id_t id, void* user_data)
{
	struct stepper* s = (struct stepper*)user_data;

	if(s->move.steps != 0)
	{
		step(s);
		s->move.steps = s->move.steps - 1;
		if(s->move.accel)
			stepper_accel(&s->move);
		return -(int64_t)s->move.delay;
	}

	else
	{
		stop(s);
		return 0;
	}	
}

void stepper_move(struct stepper* s)
{
	gpio_clr_mask((1 << s->p_motion) | (1 << s->p_move_ready));

	s->status		&= ~PICO_STATUS_MOVE_READY;
	s->status		|= PICO_STATUS_IN_MOTION;
	s->move.v_sps	= s->move.v0_sps;
	s->move.delay	= 1000000 / s->move.v_sps;	// delay in us between steps

//	stepper_print_move(&s->move);

	if(s->move.mode & JOG_MOVE)
	{
		s->dir = s->move.step_dir;
		set_dir(s);
		s->alarm_id = alarm_pool_add_alarm_in_us(	s->alarmPool,
													s->move.delay,
													jog_alarm_callback,
													(void*)s,
													true);
	}	
	else if(s->move.mode & SYNC_MOVE)
	{
		s->status |= PICO_STATUS_SYNC_READY;
		gpio_put(s->p_sync_ready, 0);
		sem_acquire_blocking(&s->sem);
		s->alarm_id = alarm_pool_add_alarm_in_us(	s->alarmPool, 
													s->move.delay,
													sync_alarm_callback,
													(void*)s, 
													true);
	}
	else
	{
		stop(s);
		return;
	}
}

void stepper_home(struct stepper*s)
{
	memset(&s->move, 0, sizeof(pincStepperMove_t));

	s->move.mode		= HOME_MOVE | JOG_MOVE;
	s->move.step_dir	= 0;
	s->move.steps		= s->config.max_steps;
	s->move.v0_sps		= s->config.min_speed;
	s->move.vf_sps		= s->config.min_speed;

	stepper_move(s);
}

void stepper_msg_handle(struct stepper* s, uint8_t* msg)
{
	printf("MSG CMD:\t0x%2X\n", msg[0]);

	switch(msg[0])
	{
		case STEPPER_CMD_MOVE:
			if(!(s->status & PICO_STATUS_IN_MOTION))
			{
				memset(&s->move, 0, sizeof(pincStepperMove_t));
				memcpy(&s->move, &msg[1], sizeof(pincStepperMove_t));
				stepper_move(s);
			}
			break;

		case STEPPER_CMD_HOME:
			if(!(s->status & PICO_STATUS_IN_MOTION))
				stepper_home(s);
			break;

		case STEPPER_CMD_CONFIG:
			memcpy(&s->config, &msg[1], sizeof(pincStepperConfig_t));
			break;
	
		case STEPPER_CMD_PRINT:

			break;

		case STEPPER_CMD_AXIS:
			// s->axis = msg->config.axis;
			break;

		case STEPPER_CMD_MODE:

			break;

		case STEPPER_CMD_SPMM:
			// s->spmm = msg->config.spmm;

			break;

		case STEPPER_CMD_JOG_SPEED:

			break;

		case STEPPER_CMD_MIN_SPEED:

			break;

		case STEPPER_CMD_MAX_SPEED:

			break;

		case STEPPER_CMD_ACCEL:

			break;

		case STEPPER_CMD_STOP:

			break;

		case STEPPER_CMD_PAUSE:

			break;

		case STEPPER_CMD_UPDATE:

			break;

		default:
			break;
	}
}