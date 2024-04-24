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

				// printf("\tPico Status:\t0x%2X\n", rx[1]);

				if(~(s->status) & PICO_STATUS_SPI_READY)
				{
					printf("Unlocking Pin %d Request\n", s->spi_request.pin);
					pin_request_unlock(&s->spi_request);
				}
			}

			break;
		}

		default:
			break;
	}
}

void stepper_spi_send(pincPiStepper* s)
{
	static uint8_t	rx[8];

	pin_request_blocking(&s->spi_request);

	spi_send(&s->pico_spi_client);
	
	spi_read_msg(&s->pico_spi_client, rx, 8);
	
	s->status	= rx[0];
	s->step_pos	= *(int*)(&rx[1]);

	printf("\tStepper Status:\t0x%2X\n", s->status);

	pin_request_reset(&s->spi_request);
}

void stepper_config(pincPiStepper* s, pincStepperConfig_t* config)
{
	stepper_lock(s);

	spi_write_msg(&s->pico_spi_client, config, sizeof(pincStepperConfig_t));

	stepper_spi_send(s);

	stepper_unlock(s);
}

void stepper_move(pincPiStepper* s, pincStepperMove_t* move)
{
	stepper_lock(s);

	spi_write_msg(&s->pico_spi_client, move, sizeof(pincStepperMove_t));
	
	stepper_spi_send(s);

	stepper_unlock(s);
}