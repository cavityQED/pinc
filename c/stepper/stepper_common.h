#ifndef STEPPER_COMMON_H
#define STEPPER_COMMON_H

#define STEPPER_CMD_CONFIG		0xA0
#define STEPPER_CMD_PRINT		0x01
#define STEPPER_CMD_AXIS		0x02
#define STEPPER_CMD_MODE		0x03
#define STEPPER_CMD_SPMM		0x04
#define STEPPER_CMD_JOG_SPEED	0x05
#define STEPPER_CMD_MIN_SPEED	0x06
#define STEPPER_CMD_MAX_SPEED	0x07
#define STEPPER_CMD_ACCEL		0x08
#define STEPPER_CMD_STOP		0x09
#define STEPPER_CMD_PAUSE		0x0A
#define STEPPER_CMD_UPDATE		0x0B
#define STEPPER_MOVE			0xF1

#define X_AXIS	0x01
#define Y_AXIS	0x02
#define Z_AXIS	0x04

#define MOTION		0x01
#define DIRECTION	0x02
#define MOVE_READY	0x04
#define SYNC_READY	0x08
#define SYNC_MODE	0x10
#define LINE_MODE	0x20
#define POS_CHANGE	0x40

#define STATUS_MOTION	0x01

typedef struct
{
	// step positions
	int x;
	int y;
	int z;
	
} p_cartesian;

typedef struct
{
	uint8_t		axis;
	uint16_t	spmm;
	uint32_t	accel;
	uint32_t	jog_speed;
	uint32_t	min_speed;
	uint32_t	max_speed;

} stepper_config_t;

typedef struct
{
	uint8_t			mode;
	p_cartesian		cur;
	p_cartesian		end;
	uint32_t		sps;
	uint32_t		rad;
	bool			cw;
	bool			pause;

} stepper_move_t;

typedef struct
{
	uint8_t		status;
	int			pos;

} stepper_info_t;

typedef struct
{
	uint16_t			cmd;	// stepper command
	stepper_config_t	config;
	stepper_move_t		move;

} stepper_msg_t;

/*
static void print_stepper_msg(StepperMsg* msg)
{
	printf("\nStepper Msg:\n");
	printf("\tCommand:\t0x%4X\n", msg->cmd);
	printf("\tArg Value:\t%d\n", msg->arg);
	printf("\tBegin:\t\t(%d, %d, %d)\n", msg->beg.x, msg->beg.y, msg->beg.z);
	printf("\tEnd:\t\t(%d, %d, %d)\n", msg->end.x, msg->end.y, msg->end.z);
	printf("\tMode:\t\t0x%2X\n", msg->mode);
	printf("\tSpeed:\t\t%u (steps/s)\n", msg->sps);
	printf("\tAccel:\t\t%u (steps/s/s)\n", msg->acc);
	printf("\tRadius:\t\t%u\n", msg->rad);
	printf("\tCW:\t\t%u\n", msg->cw);
}*/

#define XPOS	0x01
#define YPOS	0x02
#define ABOVE	0x04
#define BELOW	0x08

typedef struct
{
	bool		dir;
	bool		stop;
	uint8_t		axis;
	p_cartesian	cur;
	p_cartesian	end;
	int			slope;

} LineMove;

static inline bool pnt_cmp_cart(p_cartesian p1, p_cartesian p2)
	{return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.z == p2.z));}

static inline float slope_xy(p_cartesian cur, p_cartesian end)
	{return (float)(end.y - cur.y) / (float)(end.x - cur.x);}

static void line_step_2d(LineMove* line)
{
	if(pnt_cmp_cart(line->cur, line->end))
	{
		printf("\tSetting Stop True\n");
		line->stop = true;
		return;
	}

	int fx = (line->cur.x == line->end.x)? line->end.y : line->slope*line->cur.x;

	uint8_t mask = (line->cur.y > fx)? ABOVE : BELOW;
	mask |= (line->end.x > 0)? XPOS : 0x00;
	mask |= (line->end.y > 0)? YPOS : 0x00;

	switch(mask)
	{
		// quadrant 1
		case ABOVE | XPOS | YPOS:	line->cur.x++; line->axis = X_AXIS; line->dir = 1; break;
		case BELOW | XPOS | YPOS:	line->cur.y++; line->axis = Y_AXIS; line->dir = 1; break;

		// quadrant 2
		case ABOVE | YPOS:			line->cur.x--; line->axis = X_AXIS; line->dir = 0; break;
		case BELOW | YPOS:			line->cur.y++; line->axis = Y_AXIS; line->dir = 1; break;

		// quadrant 3
		case ABOVE:					line->cur.y--; line->axis = Y_AXIS; line->dir = 0; break;
		case BELOW:					line->cur.x--; line->axis = X_AXIS; line->dir = 0; break;
			
		// quadrant 4
		case ABOVE | XPOS:			line->cur.y--; line->axis = Y_AXIS; line->dir = 0; break;
		case BELOW | XPOS:			line->cur.x++; line->axis = X_AXIS; line->dir = 1; break;

		default:
			break;
	}
}




#endif