#ifndef STEPPER_COMMON_H
#define STEPPER_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define STEPPER_CMD_CONFIG		0xA0
#define STEPPER_CMD_PRINT		0xB1
#define STEPPER_CMD_AXIS		0xB2
#define STEPPER_CMD_MODE		0xB3
#define STEPPER_CMD_SPMM		0xB4
#define STEPPER_CMD_JOG_SPEED	0xB5
#define STEPPER_CMD_MIN_SPEED	0xB6
#define STEPPER_CMD_MAX_SPEED	0xB7
#define STEPPER_CMD_ACCEL		0xB8
#define STEPPER_CMD_STOP		0xB9
#define STEPPER_CMD_PAUSE		0xBA
#define STEPPER_CMD_UPDATE		0xBB
#define STEPPER_CMD_MOVE		0xF1

static const uint8_t PICO_STATUS_SPI_READY	= 0x08;
static const uint8_t PICO_STATUS_MOVE_READY	= 0x04;
static const uint8_t PICO_STATUS_SYNC_READY	= 0x02;
static const uint8_t PICO_STATUS_IN_MOTION	= 0x01;
static const uint8_t PICO_STATUS_DIRECTION	= 0x10;

static const uint8_t JOG_MOVE	= 0x01;
static const uint8_t LINE_MOVE	= 0x02;
static const uint8_t CURVE_MOVE	= 0x04;
static const uint8_t SYNC_MOVE	= 0x08;

typedef enum
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS,

} PINC_AXIS; 

typedef enum
{
	AUTO_CTRL,
	JOG_CTRL,
	HOME_CTRL,
	EDIT_CTRL,
	MDI_CTRL,
	MANUAL_CTRL,

} CONTROL_MODE;

typedef struct
{
	uint8_t				axis;
	uint32_t			spmm;		// steps per mm
	uint32_t			accel;		// acceleration [steps/s/s]
	uint32_t			jog_steps;	// number of steps per each jog pulse
	uint32_t			jog_speed;	// [steps/s]
	uint32_t			min_speed;	// [steps/s]
	uint32_t			max_speed;	// [steps/s]

	int					spi_client_cs;	// additional cs; -1 if unused
	uint8_t				spi_cs_change;	// true - deselect
	uint32_t			spi_bpw;		// spi bits per word
	uint32_t			spi_delay;		// spi post transfer delay in us
	uint32_t			spi_fpga_speed;	// pico spi clock speed (hz)
	uint32_t			spi_pico_speed; // fpga spi clock speed (hz)

	int					pin_status;		// status interrupt pin
	int					pin_spi_hs;		// spi handshake pin
	uint8_t				fpga_addr;		// address to read status from FPGA

	pthread_mutex_t*	spi_mutex;

} pincStepperConfig_t;

static void stepper_get_default_config(pincStepperConfig_t* config)
{
	memset(config, 0, sizeof(pincStepperConfig_t));

	uint32_t spmm = 80;
	config->spmm			= spmm;
	config->jog_steps		= spmm;
	config->jog_speed		= 60*spmm;
	config->min_speed		= 10*spmm;
	config->max_speed		= 50*spmm;

	config->spi_client_cs	= -1;
	config->spi_bpw			= 8;
	config->spi_fpga_speed	= 50000;
	config->spi_pico_speed	= 4000000;
}

typedef struct
{
	uint8_t				status;
	int					step_pos;

} pincStepperUpdate_t;







typedef struct
{
	// step positions
	int x;
	int y;
	int z;
	
} p_cartesian;

static inline bool pnt_cmp_cart(p_cartesian p1, p_cartesian p2)
	{	return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.z == p2.z));}

static inline float slope_xy(p_cartesian cur, p_cartesian end)
	{	return (float)(end.y - cur.y) / (float)(end.x - cur.x);}

static inline int pnt_axis_value(p_cartesian pnt, PINC_AXIS axis)
	{
		switch(axis)
		{
			case X_AXIS:
				return pnt.x;
			case Y_AXIS:
				return pnt.y;
			case Z_AXIS:
				return pnt.z;
			default:
				return 0;
		}
	}

static inline float length(p_cartesian cur, p_cartesian end)
	{
		float x = end.x - cur.x;
		float y = end.y - cur.y;
		float z = end.z - cur.z;

		return sqrtf(x*x + y*y + z*z);
	}

static inline uint32_t total(p_cartesian cur, p_cartesian end)
	{
		uint32_t x = abs(end.x - cur.x);
		uint32_t y = abs(end.y - cur.y);
		uint32_t z = abs(end.z - cur.z);

		return x+y+z;
	}









#define XPOS	0x01
#define YPOS	0x02
#define ABOVE	0x04
#define BELOW	0x08
#define INSIDE	0x10
#define CW		0x20
#define QUAD_1	XPOS | YPOS
#define QUAD_2	XPOS
#define QUAD_3	0x00
#define QUAD_4	YPOS

typedef enum
{
	INITIAL,
	CONSTANT,
	FINAL

} ACCEL_PHASE;

typedef struct
{
	uint8_t			mode;
	p_cartesian		cur;		// current point
	p_cartesian		end;		// end point
	p_cartesian		center;		// center of curve move
	uint32_t		v_sps;		// current speed [steps/s]
	uint32_t		v0_sps;		// start speed [steps/s]
	uint32_t		vf_sps;		// max speed [steps/s]
	uint32_t		radius;		// [steps]
	uint32_t		steps;		// used instead of cur/end for jog mode
	bool			cw;			// 0 - counterclockwise; 1 - clockwise

	uint8_t			step_axis;
	uint32_t		delay;		// delay in us between steps
	uint32_t		accel;		// acceleration [steps/s/s]
	uint32_t		a_steps;	// number of steps during accel phase
	uint8_t			a_phase;	// acceleration phase
	bool			step_dir;
	bool			stop;

} pincStepperMove_t;

static void stepper_print_move(pincStepperMove_t* m)
{
	printf("Stepper Move (%d bytes):\n", sizeof(pincStepperMove_t));
	printf("\tMode:\t\t\t0x%2X\n", m->mode);
	printf("\tCur Pos (steps):\t[%d, %d, %d]\n", m->cur.x, m->cur.y, m->cur.z);
	printf("\tEnd Pos (steps):\t[%d, %d, %d]\n", m->end.x, m->end.y, m->end.z);
	printf("\tCenter Pos (steps):\t[%d, %d, %d]\n", m->center.x, m->center.y, m->center.z);
	printf("\tSpeed [v0, v, vf]:\t[%d, %d, %d]\n", m->v0_sps, m->v_sps, m->vf_sps);
	printf("\tRadius (steps):\t\t%d\n", m->radius);
	printf("\tClockwise:\t\t%d\n", m->cw);
	printf("\tAccel:\t\t\t%d\n", m->accel);
	printf("\tAccel Steps:\t\t%d\n", m->a_steps);
	printf("\tSteps:\t\t\t%d\n\n", m->steps);

}

static void stepper_move_mm_to_steps(pincStepperMove_t* mm_move, pincStepperMove_t* steps_move, uint32_t spmm)
{
	memcpy(steps_move, mm_move, sizeof(pincStepperMove_t));

	steps_move->cur.x		= spmm * mm_move->cur.x;
	steps_move->cur.y		= spmm * mm_move->cur.y;
	steps_move->cur.z		= spmm * mm_move->cur.z;
	steps_move->end.x		= spmm * mm_move->end.x;
	steps_move->end.y		= spmm * mm_move->end.y;
	steps_move->end.z		= spmm * mm_move->end.z;
	steps_move->center.x	= spmm * mm_move->center.x;
	steps_move->center.y	= spmm * mm_move->center.y;
	steps_move->center.z	= spmm * mm_move->center.z;
	steps_move->v0_sps		= spmm * mm_move->v0_sps;
	steps_move->vf_sps		= spmm * mm_move->vf_sps;
	steps_move->accel		= spmm * mm_move->accel;
	steps_move->radius		= spmm * mm_move->radius;
	steps_move->steps		= spmm * mm_move->steps;
}

static inline void stepper_accel(pincStepperMove_t* m)
{
	if(m->steps == 0)
		return;

	uint32_t v = m->v_sps;
	
	switch(m->a_phase)
	{
		case INITIAL:
			m->a_steps++;
			v = v + m->accel/v;
			v = (v > m->vf_sps)? m->vf_sps : v;
			m->a_phase = (v == m->vf_sps)? CONSTANT : INITIAL;
			break;

		case CONSTANT:
			m->a_phase = (m->steps == m->a_steps)? FINAL : CONSTANT;
			break;

		case FINAL:
			v = v - m->accel/v;
			v = (v < m->v0_sps)? m->v0_sps : v;
			break;

		default:
			break;
	}

	m->v_sps = v;
	m->delay = 1000000 / v;
}

static void line_step_2d(pincStepperMove_t* move)
{
	if(pnt_cmp_cart(move->cur, move->end))
	{
		printf("\tSetting Stop True\n");
		move->stop = true;
		return;
	}

	float slope = slope_xy(move->cur, move->end);
	int fx = (move->cur.x == move->end.x)? move->end.y : slope*move->cur.x;

	uint8_t mask = (move->cur.y > fx)? ABOVE : BELOW;
	mask |= (move->end.x > 0)? XPOS : 0x00;
	mask |= (move->end.y > 0)? YPOS : 0x00;

	switch(mask)
	{
		case QUAD_1 | ABOVE:	move->cur.x++; move->step_axis = X_AXIS; move->step_dir = 1; break;
		case QUAD_1 | BELOW:	move->cur.y++; move->step_axis = Y_AXIS; move->step_dir = 1; break;

		case QUAD_2 | ABOVE:	move->cur.y--; move->step_axis = Y_AXIS; move->step_dir = 0; break;
		case QUAD_2 | BELOW:	move->cur.x++; move->step_axis = X_AXIS; move->step_dir = 1; break;

		case QUAD_3 | ABOVE:	move->cur.y--; move->step_axis = Y_AXIS; move->step_dir = 0; break;
		case QUAD_3 | BELOW:	move->cur.x--; move->step_axis = X_AXIS; move->step_dir = 0; break;
			
		case QUAD_4 | ABOVE:	move->cur.x--; move->step_axis = X_AXIS; move->step_dir = 0; break;
		case QUAD_4 | BELOW:	move->cur.y++; move->step_axis = Y_AXIS; move->step_dir = 1; break;

		default:
			break;
	}
}

static void curve_step_2d(pincStepperMove_t* move)
{
	if(pnt_cmp_cart(move->cur, move->end))
	{
		printf("\tSetting Stop True\n");
		move->stop = true;
		return;
	}

	int cur_x = move->cur.x - move->center.x;
	int cur_y = move->cur.y - move->center.y;
	int dxy			= (cur_x*cur_x)+(cur_y*cur_y) - (move->radius*move->radius);
	uint8_t mask	= 0x00;

	if(dxy < 0)
		mask |= INSIDE;
	if(cur_x > 0)
		mask |= XPOS;
	if(cur_y > 0)
		mask |= YPOS;
	if(move->cw)
		mask |= CW;

	switch(mask)
	{
		case QUAD_1 | INSIDE | CW:	move->cur.x++;	move->step_axis = X_AXIS;	move->step_dir = 1; break;
		case QUAD_1 | INSIDE:		move->cur.y++;	move->step_axis = Y_AXIS;	move->step_dir = 1; break;
		case QUAD_1 | CW:			move->cur.y--;	move->step_axis = Y_AXIS;	move->step_dir = 0; break;
		case QUAD_1:				move->cur.x--;	move->step_axis = X_AXIS;	move->step_dir = 0; break;

		case QUAD_2 | INSIDE | CW:	move->cur.y--;	move->step_axis = Y_AXIS;	move->step_dir = 0; break;
		case QUAD_2 | INSIDE:		move->cur.x++;	move->step_axis = X_AXIS;	move->step_dir = 1; break;
		case QUAD_2 | CW:			move->cur.x--;	move->step_axis = X_AXIS;	move->step_dir = 0; break;
		case QUAD_2:				move->cur.y++;	move->step_axis = Y_AXIS;	move->step_dir = 1; break;

		case QUAD_3 | INSIDE | CW:	move->cur.x--;	move->step_axis = X_AXIS;	move->step_dir = 0; break;
		case QUAD_3 | INSIDE:		move->cur.y--;	move->step_axis = Y_AXIS;	move->step_dir = 0; break;
		case QUAD_3 | CW:			move->cur.y++;	move->step_axis = Y_AXIS;	move->step_dir = 1; break;
		case QUAD_3:				move->cur.x++;	move->step_axis = X_AXIS;	move->step_dir = 1; break;

		case QUAD_4 | INSIDE | CW:	move->cur.y++;	move->step_axis = Y_AXIS;	move->step_dir = 1; break;
		case QUAD_4 | INSIDE:		move->cur.x--;	move->step_axis = X_AXIS;	move->step_dir = 0; break;
		case QUAD_4 | CW:			move->cur.x++;	move->step_axis = X_AXIS;	move->step_dir = 1; break;
		case QUAD_4:				move->cur.y--;	move->step_axis = Y_AXIS;	move->step_dir = 0; break;

		default:
			break;
	}
}












#endif
