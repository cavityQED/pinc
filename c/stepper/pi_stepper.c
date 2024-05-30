#include "pi_stepper.h"

void stepper_pin_isr(int gpio, int level, uint32_t tick, void* dev)
{
	pincPiStepper* stepper = (pincPiStepper*)dev;

	if(!level)
		stepper_launch_thread(stepper, stepper_read_fpga);
}

void* stepper_read_fpga(void* arg)
{
	pincPiStepper* stepper = (pincPiStepper*)arg;

	uint8_t tx[FPGA_SPI_MSG_SIZE] = {0};
	uint8_t rx[FPGA_SPI_MSG_SIZE] = {0};

	tx[0] = stepper->fpga_status_addr;

	stepper->fpga_spi_client.tr.len	= FPGA_SPI_MSG_SIZE;
	stepper->fpga_spi_client.tr.tx_buf = (unsigned long)tx;
	stepper->fpga_spi_client.tr.rx_buf = (unsigned long)rx;

	spi_send(&stepper->fpga_spi_client);

	signal_update(&stepper->status_sig, rx[1]);

	if(stepper->status_sig.low & PICO_STATUS_SPI_READY)
		pin_request_post(&stepper->spi_request);
	if(stepper->status_sig.high & PICO_STATUS_SPI_READY)
		pin_request_unlock(&stepper->spi_request);
	if(stepper->status_sig.low & PICO_STATUS_SYNC_READY)
		sem_post(&stepper->sync_sem);

	if(stepper->status_sig.high & PICO_STATUS_IN_MOTION)
		stepper_update(stepper);
}

void stepper_write_msg(pincPiStepper* stepper, uint8_t cmd, void* src, uint32_t bytes)
{
	uint32_t msg_size = bytes + 1;
	stepper->msg.tx = malloc(msg_size);
	stepper->msg.rx = malloc(msg_size);

	stepper->msg.tx[0] = cmd;
	if(src)
		memcpy(&stepper->msg.tx[1], src, bytes);
	else
		memset(&stepper->msg.tx[1], 0, bytes);

	stepper->pico_spi_client.tr.len		= msg_size;
	stepper->pico_spi_client.tr.tx_buf	= (unsigned long)stepper->msg.tx;
	stepper->pico_spi_client.tr.rx_buf	= (unsigned long)stepper->msg.rx;
}

void stepper_send_msg(pincPiStepper* stepper)
{
	pin_request_blocking(&stepper->spi_request);

	spi_send(&stepper->pico_spi_client);

	stepper_read_msg(stepper);

	pin_request_reset(&stepper->spi_request);
}

void stepper_read_msg(pincPiStepper* stepper)
{
	pincStepperUpdate_t update;

	memcpy(&update, stepper->msg.rx, sizeof(pincStepperUpdate_t));

	stepper->step_pos	= update.step_pos;

	free(stepper->msg.tx);
	free(stepper->msg.rx);

	stepper_print(stepper);
}

void stepper_config(pincPiStepper* s, pincStepperConfig_t* config)
{
	stepper_lock(s);

	memcpy(&s->config, config, sizeof(pincStepperConfig_t));

	stepper_write_msg(	s,
						STEPPER_CMD_CONFIG,
						&s->config,
						sizeof(pincStepperConfig_t));

	stepper_send_msg(s);

	stepper_unlock(s);
}

void stepper_move(pincPiStepper* s, pincStepperMove_t* move)
{
	printf("Stepper Move\n");

	stepper_lock(s);

	stepper_write_msg(	s,
						STEPPER_CMD_MOVE,
						move,
						sizeof(pincStepperMove_t));

	stepper_send_msg(s);

	stepper_unlock(s);
}

void stepper_jog(pincPiStepper* s)
{
	printf("Stepper Jog\n");

	stepper_lock(s);

	stepper_write_msg(	s,
						STEPPER_CMD_MOVE,
						&s->jog_move,
						sizeof(pincStepperMove_t));

	stepper_send_msg(s);

	stepper_unlock(s);
}

void stepper_update(pincPiStepper* s)
{
	printf("Stepper Update\n");

	stepper_lock(s);

	stepper_write_msg(	s,
						STEPPER_CMD_UPDATE,
						NULL,
						sizeof(pincStepperUpdate_t));

	stepper_send_msg(s);

	stepper_unlock(s);
}

void stepper_print(pincPiStepper* s)
{
	printf("Axis %X:\n\tPosition (steps):\t%d\n\n", s->config.axis, s->step_pos);
}

