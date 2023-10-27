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
	lcdSegment			lcd_seg;
	lcd_font_t*			lcd_font;
	pthread_mutex_t		mutex;

} stepper_t;

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev);

void* stepper_spi_send(void* dev);
void* stepper_update_loop(void* dev);

void stepper_update(stepper_t* s);
void stepper_config(stepper_t* s);
void stepper_move(stepper_t* s, stepper_move_t* move);
void stepper_pause(stepper_t* s, bool pause);

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


#endif