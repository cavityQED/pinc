#ifndef STEPPER_COMMON_H
#define STEPPER_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

static const uint8_t PICO_STATUS_SPI_READY	= 0x01;
static const uint8_t PICO_STATUS_MOVE_READY	= 0x02;
static const uint8_t PICO_STATUS_SYNC_READY	= 0x04;
static const uint8_t PICO_STATUS_IN_MOTION	= 0x08;
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

	pthread_mutex_t*	spi_mutex;
	pthread_mutex_t*	pin_req_mutex;

} pincStepperConfig_t;

static void stepper_get_default_config(pincStepperConfig_t* config)
{
	memset(config, 0, sizeof(pincStepperConfig_t));

	uint32_t spmm = 80;
	config->spmm			= spmm;
	config->jog_steps		= spmm;
	config->jog_speed		= 40*spmm;
	config->min_speed		= 10*spmm;
	config->max_speed		= 50*spmm;

	config->spi_client_cs	= -1;
	config->spi_bpw			= 8;
	config->spi_fpga_speed	= 500000;
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
	{return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.z == p2.z));}

static inline float slope_xy(p_cartesian cur, p_cartesian end)
	{return (float)(end.y - cur.y) / (float)(end.x - cur.x);}









#define XPOS	0x01
#define YPOS	0x02
#define ABOVE	0x04
#define BELOW	0x08

typedef struct
{
	uint8_t			mode;
	p_cartesian		cur;		// current point
	p_cartesian		end;		// end point
	uint32_t		v_sps;		// speed [steps/s]
	uint32_t		radius;		// [steps]
	uint32_t		steps;		// used instead of cur/end for jog mode
	bool			cw;			// 0 - counterclockwise; 1 - clockwise

	uint8_t			step_axis;
	uint32_t		delay;		// delay in us between steps
	bool			step_dir;
	bool			stop;

} pincStepperMove_t;

static void stepper_print_move(pincStepperMove_t* m)
{
	printf("Stepper Move (%d bytes):\n", sizeof(pincStepperMove_t));
	printf("\tMode:\t\t\t0x%2X\n", m->mode);
	printf("\tCur Pos (steps):\t[%d, %d, %d]\n", m->cur.x, m->cur.y, m->cur.z);
	printf("\tEnd Pos (steps):\t[%d, %d, %d]\n", m->end.x, m->end.y, m->end.z);
	printf("\tSpeed (steps/s):\t%d\n", m->v_sps);
	printf("\tRadius (steps):\t\t%d\n", m->radius);
	printf("\tClockwise:\t\t%d\n", m->cw);
	printf("\tSteps:\t\t\t%d\n\n", m->steps);

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
		// quadrant 1
		case ABOVE | XPOS | YPOS:	move->cur.x++; move->step_axis = X_AXIS; move->step_dir = 1; break;
		case BELOW | XPOS | YPOS:	move->cur.y++; move->step_axis = Y_AXIS; move->step_dir = 1; break;

		// quadrant 2
		case ABOVE | YPOS:			move->cur.x--; move->step_axis = X_AXIS; move->step_dir = 0; break;
		case BELOW | YPOS:			move->cur.y++; move->step_axis = Y_AXIS; move->step_dir = 1; break;

		// quadrant 3
		case ABOVE:					move->cur.y--; move->step_axis = Y_AXIS; move->step_dir = 0; break;
		case BELOW:					move->cur.x--; move->step_axis = X_AXIS; move->step_dir = 0; break;
			
		// quadrant 4
		case ABOVE | XPOS:			move->cur.y--; move->step_axis = Y_AXIS; move->step_dir = 0; break;
		case BELOW | XPOS:			move->cur.x++; move->step_axis = X_AXIS; move->step_dir = 1; break;

		default:
			break;
	}
}













#endif
