#ifndef STEPPER_COMMON_H
#define STEPPER_COMMON_H

#define STEPPER_CMD_PRINT			0x01
#define STEPPER_CMD_AXIS			0x02
#define STEPPER_CMD_MODE			0x03
#define STEPPER_CMD_SPMM			0x04
#define STEPPER_CMD_JOG_SPEED		0x05
#define STEPPER_CMD_MIN_SPEED		0x06
#define STEPPER_CMD_MAX_SPEED		0x07
#define STEPPER_CMD_ACCELERATION	0x08
#define STEPPER_CMD_STOP			0x09
#define STEPPER_CMD_MOVE			0x0A
#define STEPPER_CMD_PAUSE			0x0B
#define STEPPER_CMD_RECEIVE			0x0C

#define X_AXIS	0x01
#define Y_AXIS	0x02
#define Z_AXIS	0x04

#define MOTION		0x01
#define DIRECTION	0x02
#define MOVE_READY	0x04
#define SYNC_READY	0x08
#define SYNC_MODE	0x10
#define LINE_MODE	0x20

typedef struct
{
	// step positions
	int x;
	int y;
	int z;
	
} p_cartesian;

typedef struct
{
	uint16_t		cmd;	// stepper command
	int				arg;	// command argument
	uint8_t			mode;
	p_cartesian		beg;	// begin point
	p_cartesian		end;	// end point
	uint32_t		sps;	// speed (steps/s)
	uint32_t		acc;	// aceleration (steps/s/s)
	uint32_t		rad;	// radius; line mode if 0
	uint8_t			cw;		// clockwise if > 0, counterclockwise if 0

} StepperMsg;

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
}

#endif