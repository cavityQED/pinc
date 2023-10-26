#ifndef HOST_STEPPER_H
#define HOST_STEPPER_H

#include "io/signal.h"
#include "io/pcf8574.h"
#include "spi/host_spi.h"
#include "lcd/st7735.h"

#include "host_common.h"

typedef struct
{
	uint8_t				status;
	int					step_pos;
	stepper_config_t	config;
	PCF					pcf_sig;
	spiMsg				spi_msg;
	spiHost*			spi_host;
	pin_request_t		spi_req;
	pin_request_t		sync_req;
	stepper_msg_t		cmd_msg;
	pthread_mutex_t		mutex;

} stepper_t;

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev);

void* stepper_spi_send(void* dev);
void* stepper_update_loop(void* dev);

void stepper_update(stepper_t* s);
void stepper_config(stepper_t* s);
void stepper_move(stepper_t* s, stepper_move_t* move);

void stepper_lcd_draw_pos(stepper_t* s);

static inline void stepper_init(stepper_t* s, stepper_config_t* config)
{
	memcpy(&s->config, config, sizeof(stepper_config_t));
	pthread_mutex_init(&s->mutex, NULL);

	stepper_config(s);
}

static inline void stepper_lock(stepper_t* s)
{
	pthread_mutex_lock(&s->mutex);
}

static inline void stepper_unlock(stepper_t* s)
{
	pthread_mutex_unlock(&s->mutex);
}

/*
static void stepper_stop(Stepper* s) {stepper_send_cmd(s, STEPPER_CMD_STOP, s->step_pos);}
static inline void stepper_set_mode			(Stepper* s, uint8_t mode)	{stepper_send_cmd(s, STEPPER_CMD_MODE, mode);}
static inline void stepper_set_spmm			(Stepper* s, uint16_t spmm)	{stepper_send_cmd(s, STEPPER_CMD_SPMM, spmm);}
static inline void stepper_set_accel		(Stepper* s, uint32_t acc)	{stepper_send_cmd(s, STEPPER_CMD_ACCEL, acc);}
static inline void stepper_set_jog_speed	(Stepper* s, uint32_t sps)	{stepper_send_cmd(s, STEPPER_CMD_JOG_SPEED, sps);}
static inline void stepper_set_min_speed	(Stepper* s, uint32_t sps)	{stepper_send_cmd(s, STEPPER_CMD_MIN_SPEED, sps);}
static inline void stepper_set_max_speed	(Stepper* s, uint32_t sps)	{stepper_send_cmd(s, STEPPER_CMD_MAX_SPEED, sps);}
static inline void stepper_pause			(Stepper* s, bool pause)	{stepper_send_cmd(s, STEPPER_CMD_PAUSE, pause);}
static inline void stepper_update			(Stepper* s)				{stepper_send_cmd(s, STEPPER_CMD_UPDATE, s->step_pos);}
*/



#endif