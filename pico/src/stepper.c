#include "stepper.h"

void stepper_init(struct stepper* s)
{
	uint mask = (1 << s->p_step) | (1 << s->p_dir) | (1 << s->p_ena) | (1 << s->p_motion);
	gpio_init_mask(mask);
	gpio_set_dir_out_masked(mask);
	gpio_put(s->p_ena, 0);
	gpio_put(s->p_motion, 1);

	sem_init(&s->sem, 0, 1);
}

void set_dir(struct stepper* s)
{
	gpio_put(s->p_dir, s->dir);
	if(s->dir)
		s->status |= DIRECTION;
	else
		s->status &= ~DIRECTION;
}

void step(struct stepper* s)
{
	gpio_put(s->p_step, 1);
	int add = (s->dir)? 1 : -1;
	s->pos += add;
	s->status |= POS_CHANGE;
	sleep_us(8);
	gpio_put(s->p_step, 0);
}

void stop(struct stepper* s)
{
	printf("\tStopping\n\n");
	cancel_repeating_timer(s->timer);
	s->status &= ~POS_CHANGE;
	s->status &= ~MOTION;
	s->status |= MOVE_READY;

	gpio_put(s->p_motion, 1);
}

bool line_timer_cb(picoTimer_t* timer)
{
	static struct stepper* s;
	s = (struct stepper*)timer->user_data;
	
	line_step_2d(&s->line);
	s->dir = s->line.dir;
	set_dir(s);

	if(s->line.stop)
	{
		stop(s);
		return false;
	}

	if(s->axis == s->line.axis)
		step(s);

	return true;
}

void stepper_line_move(struct stepper* s)
{
	printf("\tLine Move\n");
	s->status	&= ~MOVE_READY;
	s->status	|= MOTION;

	if(s->status & SYNC_MODE)
		sem_acquire_blocking(&s->sem);

	printf("\tMoving:\n");
	printf("\tDelay:\t%d\n", s->delay);
	printf("\tCur:\t(%d, %d, %d)\n", s->line.cur.x, s->line.cur.y, s->line.cur.z);
	printf("\tEnd:\t(%d, %d, %d)\n", s->line.end.x, s->line.end.y, s->line.end.z);
	printf("\tAxis:\t0x%2X\n", s->axis);

	alarm_pool_add_repeating_timer_us(s->alarmPool, -(s->delay), line_timer_cb, (void*)s, s->timer);
	gpio_put(s->p_motion, 0);
}

void stepper_move(struct stepper* s, stepper_move_t* m)
{
	if(m->mode == LINE_MODE)
	{
		s->line.stop = false;
		s->line.cur = m->cur;
		s->line.end = m->end;
		s->line.slope = slope_xy(m->cur, m->end);
		s->delay = 1000000 / m->sps;
		stepper_line_move(s);
	}
}

void stepper_msg_handle(struct stepper* s, stepper_msg_t* msg)
{
	static stepper_move_t move;
	static stepper_config_t config;
	printf("MSG CMD:\t0x%4X\n", msg->cmd);
	switch(msg->cmd)
	{
		case STEPPER_MOVE:
			if(s->status & MOTION)
				return;
			memcpy(&move, msg, sizeof(move));
			stepper_move(s, &move);
			break;

		case STEPPER_CMD_CONFIG:
			memcpy(&config, msg, sizeof(config));
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