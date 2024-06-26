#include "host_stepper.h"

void stepper_init(stepper_t* s, stepper_config_t* config)
{
	pthread_mutex_init(&s->mutex, NULL);

	stepper_event_t* event = malloc(sizeof(stepper_event_t));
	event->cmd = STEPPER_CMD_CONFIG;
	event->s = s;
	memcpy(&s->config, config, sizeof(stepper_config_t));

	pthread_t thread;
	pthread_create(&thread, NULL, stepper_event, event);
	pthread_detach(thread);
}

void stepper_status_isr(int gpio, int level, uint32_t tick, void* dev)
{
	stepper_t* s = (stepper_t*)dev;

	fpga_get_signal(&s->fpga_sig);

	Node* node = node_find(s->req_tree->root, s->fpga_sig.sig.mask);
	if(node)
	{
		request_t* req = (request_t*)node->data;
		req->func(req);
	}
}

void* stepper_spi_send(void* dev)
{
	stepper_info_t	info;

	stepper_t* s = (stepper_t*)dev;

	stepper_lock(s);

	pin_request_blocking(&s->spi_req);

	spi_send(s->spi_host, &s->spi_msg);
	spi_read_msg(&s->spi_msg, (uint8_t*)&info, sizeof(stepper_info_t));
	s->step_pos = info.pos;
	s->status = info.status;
	pin_request_reset(&s->spi_req);

	stepper_unlock(s);

	return NULL;
}

void StepperSPISend(Stepper* s, spiHost* host, spiMsg* msg)
{
	
}


void stepper_update(stepper_t* s)
{
	uint16_t bytes = sizeof(stepper_info_t);
	stepper_info_t info;
	info.cmd = STEPPER_CMD_UPDATE;
	stepper_lock(s);
	s->spi_msg.tx = malloc(bytes);
	s->spi_msg.rx = malloc(bytes);
	s->spi_msg.len = bytes;
	memcpy(s->spi_msg.tx, &info, bytes);
	stepper_unlock(s);
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
	s->config.cmd = STEPPER_CMD_CONFIG;
	spi_write_msg(&s->spi_msg, &s->config, sizeof(stepper_config_t));
	stepper_unlock(s);

	stepper_spi_send(s);
}

void stepper_move(stepper_t* s)
{
	// if(s->status & MOTION)
	// 	return;

	stepper_lock(s);
	s->move.cmd = STEPPER_MOVE;
	spi_write_msg(&s->spi_msg, &s->move, sizeof(stepper_move_t));
	stepper_unlock(s);

	stepper_spi_send(s);
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


void* stepper_event(void* data)
{
	stepper_event_t* e = (stepper_event_t*)data;

	if(!e->s)
		return NULL;

	switch(e->cmd)
	{
		case STEPPER_MOVE:
			stepper_move(e->s);
			break;
		case STEPPER_CMD_CONFIG:
			stepper_config(e->s);
			break;
		default:
			break;
	}

	free(data);
	return NULL;
}
