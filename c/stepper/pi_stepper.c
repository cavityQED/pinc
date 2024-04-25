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
	static pincStepperUpdate_t	update;

	pin_request_blocking(&s->spi_request);

	spi_send(&s->pico_spi_client);
	
	memcpy(&update, (void*)s->pico_spi_client.tr.rx_buf, sizeof(pincStepperUpdate_t));

	s->status	= update.status;
	s->step_pos	= update.step_pos;
	
	pin_request_reset(&s->spi_request);
}

void stepper_cmd(pincPiStepper* s, uint8_t cmd, void* data, uint32_t bytes)
{
	uint32_t msg_size = bytes + 1;
	uint8_t* tx = malloc(msg_size);
	uint8_t* rx = malloc(msg_size);

	tx[0] = cmd;
	memcpy(&tx[1], data, bytes);

	s->pico_spi_client.tr.len		= msg_size;
	s->pico_spi_client.tr.tx_buf	= (unsigned long)tx;
	s->pico_spi_client.tr.rx_buf	= (unsigned long)rx;

	stepper_spi_send(s);

	free(tx);
	free(rx);
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