#include "pi_stepper.h"

void stepper_pin_isr(int gpio, int level, uint32_t tick, void* dev)
{
	pincPiStepper*		s 		= (pincPiStepper*)dev;
	pincSPIclient_t*	client	= &s->fpga_spi_client;

	uint8_t tx[2] = {0,0};
	uint8_t rx[2] = {0,0};

	switch(gpio)
	{
		case X_STATUS_INTERRUPT:
			tx[0] = X_FPGA_STATUS_ADDR;
			break;
		case Y_STATUS_INTERRUPT:
			tx[0] = Y_FPGA_STATUS_ADDR;
			break;

		default:
			break;
	}


	if(!level) // start spi
	{
		spi_write_msg(client, tx, 2);

		spi_send(client);

		spi_read_msg(client, rx, 2);

		uint8_t pre_status	= s->status;
		uint8_t del_status	= pre_status ^ rx[1];
		uint8_t high_flip	= del_status & rx[1];
		uint8_t low_flip	= del_status & ~rx[1];
		s->status = rx[1];

		if(low_flip & PICO_STATUS_SPI_READY)
			pin_request_post(&s->spi_request);
		else if(low_flip & PICO_STATUS_SYNC_READY)
			sem_post(&s->sync_sem);
		else if(high_flip & PICO_STATUS_IN_MOTION)
		{
			if(!s->spi_request.fired)
				stepper_update(s);
		}
	}
}

void stepper_spi_send(pincPiStepper* s)
{
	pincStepperUpdate_t	update;

	pin_request_blocking(&s->spi_request);

	spi_send(&s->pico_spi_client);
	
	memcpy(&update, s->rx, sizeof(pincStepperUpdate_t));

	s->step_pos	= update.step_pos;

	free(s->tx);
	free(s->rx);

	stepper_print(s);
	
	pin_request_reset(&s->spi_request);
}

void* stepper_thread_routine(void* arg)
{
	pincPiStepper* stepper = (pincPiStepper*)arg;

	stepper_lock(stepper);

	stepper_spi_send(stepper);

	stepper_unlock(stepper);
}

void stepper_cmd(pincPiStepper* s, uint8_t cmd, void* data, uint32_t bytes)
{
	uint32_t msg_size = bytes + 1;
	s->tx = malloc(msg_size);
	s->rx = malloc(msg_size);

	s->tx[0] = cmd;
	if(data)
		memcpy(&s->tx[1], data, bytes);
	else
		memset(&s->tx[1], 0, bytes);

	s->pico_spi_client.tr.len		= msg_size;
	s->pico_spi_client.tr.tx_buf	= (unsigned long)s->tx;
	s->pico_spi_client.tr.rx_buf	= (unsigned long)s->rx;

	pthread_t thread;
	pthread_create(&thread, NULL, stepper_thread_routine, s);
	pthread_detach(thread);
}

void stepper_config(pincPiStepper* s, pincStepperConfig_t* config)
{
	stepper_lock(s);

	memcpy(&s->config, config, sizeof(pincStepperConfig_t));

	stepper_cmd(s, STEPPER_CMD_CONFIG, config, sizeof(pincStepperConfig_t));

	stepper_unlock(s);
}

void stepper_move(pincPiStepper* s, pincStepperMove_t* move)
{
	stepper_lock(s);

	stepper_cmd(s, STEPPER_CMD_MOVE, move, sizeof(pincStepperMove_t));

	stepper_unlock(s);
}

void stepper_jog(pincPiStepper* s)
{
	stepper_lock(s);

	stepper_cmd(s, STEPPER_CMD_MOVE, &s->jog_move, sizeof(pincStepperMove_t));

	stepper_unlock(s);
}

void stepper_update(pincPiStepper* s)
{
	stepper_lock(s);

	stepper_cmd(s, STEPPER_CMD_UPDATE, NULL, sizeof(pincStepperUpdate_t));

	stepper_unlock(s);
}

void stepper_print(pincPiStepper* s)
{
	printf("Axis %X:\n\tPosition (steps):\t%d\n\n", s->config.axis, s->step_pos);
}

