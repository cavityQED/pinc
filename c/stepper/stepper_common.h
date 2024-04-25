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

#define MOTION		0x01
#define DIRECTION	0x02
#define MOVE_READY	0x04
#define SYNC_READY	0x08
#define SYNC_MODE	0x10
#define LINE_MODE	0x20
#define JOG_MODE	0x40
#define POS_CHANGE	0x80

#define STATUS_MOTION	0x01



typedef enum
{
	X_AXIS = 0x01,
	Y_AXIS = 0x02,
	Z_AXIS = 0x04

} PINC_AXIS; 

typedef enum
{
	JOG,
	LINE,
	CURVE

} MOVE_MODE;

typedef struct
{
	PINC_AXIS		axis;
	uint32_t		spmm;		// steps per mm
	uint32_t		accel;		// acceleration [steps/s/s]
	uint32_t		jog_steps;	// number of steps per each jog pulse
	uint32_t		jog_speed;	// [steps/s]
	uint32_t		min_speed;	// [steps/s]
	uint32_t		max_speed;	// [steps/s]

} pincStepperConfig_t;

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
	MOVE_MODE		mode;
	p_cartesian		cur;	// current point
	p_cartesian		end;	// end point
	uint32_t		v_sps;	// speed [steps/s]
	uint32_t		radius;	// [steps]
	bool			cw;		// 0 - counterclockwise; 1 - clockwise

	PINC_AXIS		step_axis;
	bool			step_dir;
	bool			stop;

} pincStepperMove_t;

static void stepper_print_move(pincStepperMove_t* m)
{
	printf("Stepper Move:\n");
	printf("\tMode:\t\t\t0x%2X\n", m->mode);
	printf("\tCur Pos (steps):\t[%d, %d, %d]\n", m->cur.x, m->cur.y, m->cur.z);
	printf("\tEnd Pos (steps):\t[%d, %d, %d]\n", m->end.x, m->end.y, m->end.z);
	printf("\tSpeed (steps/s):\t%d\n", m->v_sps);
	printf("\tRadius (steps):\t%d\n", m->radius);
	printf("\tClockwise:\t\t%d\n\n", m->cw);
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