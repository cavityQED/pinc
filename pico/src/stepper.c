#include "stepper.h"

void stepper_init(struct stepper* s)
{
	uint mask = (1 << s->p_step)		| 
				(1 << s->p_dir)			| 
				(1 << s->p_ena)			| 
				(1 << s->p_motion)		| 
				(1 << s->p_sync_ready)	|
				(1 << s->p_move_ready);

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
	printf("\tStopping\n\n");
	cancel_repeating_timer(s->timer);
	s->status &= ~PICO_STATUS_IN_MOTION;
	s->status |= PICO_STATUS_MOVE_READY;

	gpio_set_mask((1 << s->p_motion) | (1 << s->p_sync_ready) | (1 << s->p_move_ready));
}

bool sync_timer_cb(picoTimer_t* timer)
{
	struct stepper* s = (struct stepper*)timer->user_data;

	if(s->move.mode & LINE_MOVE)
		line_step_2d(&s->move);
	else if(s->move.mode & CURVE_MOVE)
		curve_step_2d(&s->move);

	if(s->move.stop)
	{
		stop(s);
		return false;
	}
	else if(s->axis == s->move.step_axis)
	{
		s->dir = s->move.step_dir;
		set_dir(s);
		step(s);
	}
	
	return true;
}

int64_t accel_alarm_callback(alarm_id_t id, void* user_data)
{
	struct stepper* s = (struct stepper*)user_data;

	if(s->move.steps != 0)
	{
		s->dir = s->move.step_dir;
		set_dir(s);
		step(s);
		s->move.steps = s->move.steps - 1;
		stepper_accel(&s->move);
		return -(int64_t)s->move.delay;
	}

	else
	{
		stop(s);
		return 0;
	}
}

bool jog_timer_cb(picoTimer_t* timer)
{
	static struct stepper* s;
	s = (struct stepper*)timer->user_data;

	if(s->move.steps != 0)
	{
		s->dir = s->move.step_dir;
		set_dir(s);
		step(s);
		s->move.steps = s->move.steps - 1;
		return true;
	}
	else
	{
		stop(s);
		return false;
	}
}

void stepper_move(struct stepper* s)
{
	if(s->status & PICO_STATUS_IN_MOTION)
		return;
	else
		stepper_print_move(&s->move);

	gpio_clr_mask((1 << s->p_motion) | (1 << s->p_move_ready));

	s->status		&= ~PICO_STATUS_MOVE_READY;
	s->status		|= PICO_STATUS_IN_MOTION;
	s->move.v_sps	= s->move.v0_sps;
	s->move.delay	= 1000000 / s->move.v_sps;	// delay in us between steps

	if(s->move.mode & JOG_MOVE)
		if(s->move.accel != 0)
		{
			s->move.a_phase = ACCEL;
			s->move.a_steps = 0;
			alarm_pool_add_alarm_in_us(	s->alarmPool,
										s->move.delay,
										accel_alarm_callback,
										(void*)s,
										true);
		}
		else
			alarm_pool_add_repeating_timer_us(	s->alarmPool, 
												-(int)(s->move.delay),
												jog_timer_cb,
												(void*)s, 
												s->timer);
	else if(s->move.mode & SYNC_MOVE)
	{
		gpio_put(s->p_sync_ready, 0);
		sem_acquire_blocking(&s->sem);
		alarm_pool_add_repeating_timer_us(	s->alarmPool, 
											-(int)(s->move.delay),
											sync_timer_cb,
											(void*)s, 
											s->timer);
	}
	else
	{
		stop(s);
		return;
	}

}

void stepper_msg_handle(struct stepper* s, uint8_t* msg)
{
	static pincStepperConfig_t config;

	printf("MSG CMD:\t0x%2X\n", msg[0]);

	switch(msg[0])
	{
		case STEPPER_CMD_MOVE:
			memset(&s->move, 0, sizeof(pincStepperMove_t));
			memcpy(&s->move, &msg[1], sizeof(pincStepperMove_t));
			stepper_move(s);
			break;

		case STEPPER_CMD_CONFIG:
			memcpy(&config, &msg[1], sizeof(pincStepperConfig_t));
			s->axis			= config.axis;
			s->spmm			= config.spmm;
			s->accel		= config.accel;
			s->jog_speed	= config.jog_speed;
			s->min_speed	= config.min_speed;
			s->max_speed	= config.max_speed;
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