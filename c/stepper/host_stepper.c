#include "host_stepper.h"

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev)
{
	stepper_t* s = (stepper_t*)dev;

	pcf_get_signal(&s->pcf_sig);

	if(request_check(s->spi_req.req, &s->pcf_sig.sig))
		pin_request_ack(&s->spi_req);


	if(s->pcf_sig.sig.low & STATUS_MOTION)
	{
		printf("Motion Started\n");

		pthread_t thread;
		pthread_create(&thread, NULL, stepper_update_loop, s);
		pthread_detach(thread);
	}
}

void* stepper_spi_send(void* dev)
{
	stepper_info_t	info;

	stepper_t* s = (stepper_t*)dev;

	stepper_lock(s);

	spi_write_msg(&s->spi_msg, (uint8_t*)&s->cmd_msg, sizeof(s->cmd_msg));

	pin_request_blocking(&s->spi_req);

	spi_send(s->spi_host, &s->spi_msg);

	spi_read_msg(&s->spi_msg, (uint8_t*)&info, sizeof(stepper_info_t));
	s->step_pos = info.pos;
	s->status = info.status;

	pin_request_reset(&s->spi_req);

	stepper_unlock(s);

	return NULL;
}

void stepper_update(stepper_t* s)
{
	s->cmd_msg.cmd = STEPPER_CMD_UPDATE;
	stepper_spi_send(s);
}

void* stepper_update_loop(void* dev)
{
	stepper_t* s = (stepper_t*)dev;
	stepper_update(s);

	while(s->status & MOTION)
	{
		gpioDelay(10000);
		stepper_update(s);
		stepper_lock(s);
		stepper_lcd_draw_pos(s);
		stepper_unlock(s);
	}

	return NULL;
}

void stepper_config(stepper_t* s)
{
	stepper_lock(s);
	s->cmd_msg.cmd = STEPPER_CMD_CONFIG;
	memset(&s->cmd_msg.config, 0, sizeof(stepper_config_t));
	memcpy(&s->cmd_msg.config, &s->config, sizeof(stepper_config_t));
	stepper_unlock(s);

	pthread_t thread;
	pthread_create(&thread, NULL, stepper_spi_send, s);
	pthread_detach(thread);
}

void stepper_move(stepper_t* s, stepper_move_t* move)
{
	if(s->status & MOTION)
		return;

	stepper_lock(s);
	s->cmd_msg.cmd = STEPPER_MOVE;
	memset(&s->cmd_msg.move, 0, sizeof(stepper_move_t));
	memcpy(&s->cmd_msg.move, move, sizeof(stepper_move_t));
	stepper_unlock(s);

	pthread_t thread;
	pthread_create(&thread, NULL, stepper_spi_send, s);
	pthread_detach(thread);
}

void stepper_lcd_draw_pos(stepper_t* s)
{
	float pos = (float)s->step_pos/s->config.spmm;
	int len = snprintf(NULL, 0, "%5.3f", pos);
	char* str = malloc(len + 1);
	snprintf(str, len + 1, "%5.3f", pos);

	lcd_draw_string(&s->lcd_seg, s->lcd_font, str);

	free(str);
}