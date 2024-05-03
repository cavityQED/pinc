#include "pi_stepper.h"

void stepper_pin_isr(int gpio, int level, uint32_t tick, void* dev)
{
	static pincSPIclient_t* client;

	pincPiStepper* s = (pincPiStepper*)dev;

	switch(gpio)
	{
		case X_STATUS_INT:
		{
			static uint8_t tx[2] = {0,0};
			static uint8_t rx[2] = {0,0};

			client = &s->fpga_spi_client;

			if(!level) // start spi
			{
				spi_write_msg(client, tx, 2);

				spi_send(client);

				spi_read_msg(client, rx, 2);

				s->status = rx[1];

				if(~(s->status) & PICO_STATUS_SPI_READY)
					pin_request_unlock(&s->spi_request);
			}

			break;
		}

		default:
			break;
	}
}

void stepper_spi_send(pincPiStepper* s)
{
	pincStepperUpdate_t	update;

	pin_request_blocking(&s->spi_request);

	spi_send(&s->pico_spi_client);
	
	memcpy(&update, s->rx, sizeof(pincStepperUpdate_t));

	s->status	= update.status;
	s->step_pos	= update.step_pos;

	free(s->tx);
	free(s->rx);
	
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
	memcpy(&s->tx[1], data, bytes);

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

	stepper_cmd(s, STEPPER_CMD_CONFIG, config, sizeof(pincStepperConfig_t));

	stepper_unlock(s);
}

void stepper_move(pincPiStepper* s, pincStepperMove_t* move)
{
	stepper_lock(s);

	stepper_cmd(s, STEPPER_MOVE, move, sizeof(pincStepperMove_t));

	stepper_unlock(s);
}

void stepper_jog(pincPiStepper* s)
{
	stepper_lock(s);

	stepper_cmd(s, STEPPER_MOVE, &s->jog_move, sizeof(pincStepperMove_t));

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
	printf("Axis %X Update:\n", s->config.axis);
	printf("\tStatus:\t\t%X\n", s->status);
	printf("\tPosition (steps):\t%d\n\n", s->step_pos);
}

