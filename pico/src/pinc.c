#include "pinc.h"

static void print_buf(uint8_t* buf, int len)
{
	printf("\nBytes:");
	for(int i = 0; i < len; i++)
	{
		if(!(i % 8))
			printf("\n\t");
		printf("%u: %2X\t", i, buf[i]);
	}

	printf("\n");
}

static void gpio_isr(uint pin, uint32_t event_mask)
{
	switch(pin)
	{
		case PICO_PIN_SPI_MSG_REQ:
			if(event_mask & GPIO_IRQ_EDGE_RISE)
				queue_try_add(&spiQueue, &spi);
			else if(event_mask & GPIO_IRQ_EDGE_FALL)
			{	
				// spi transfer complete
				pio_spi_client_disable(&spi);
				queue_try_add(&msgQueue, spi.rx_buf);
			}
			break;

		case PICO_PIN_SYNC_SIGNAL:
			if(event_mask & GPIO_IRQ_EDGE_RISE)
				sem_release(&motor.sem);
			break;
		default:
			break;
	}
}

static void get_spi_msg()
{
	queue_peek_blocking(&spiQueue, &spi);

	static pincStepperUpdate_t update;
	update.status = motor.status;
	update.step_pos = motor.pos;

	printf("Status:\t%X\n", motor.status);
	printf("Position:\t%d\n", motor.pos);

	memset(spi_in_buf, 0, MAX_SPI_TRANSFER_LENGTH);
	memset(spi_out_buf, 0, MAX_SPI_TRANSFER_LENGTH);
	memcpy(spi_out_buf, &update, sizeof(pincStepperUpdate_t));
	spi.tx_buf = spi_out_buf;

	pio_spi_client_transfer_blocking(&spi);
	pio_spi_client_disable(&spi);
	queue_remove_blocking(&spiQueue, &spi);
}

static void core1_main()
{
	motor.status		= PICO_STATUS_MOVE_READY;
	motor.p_step		= PICO_PIN_STEP;
	motor.p_dir			= PICO_PIN_DIR;
	motor.p_ena			= PICO_PIN_ENABLE;
	motor.p_motion		= PICO_PIN_MOTION;
	motor.p_sync_ready	= PICO_PIN_SYNC_READY;
	motor.p_move_ready	= PICO_PIN_MOVE_READY;
	motor.spmm			= 800;
	motor.move.delay	= 125;
	motor.alarmPool		= alarm_pool_create(1, 2);
	motor.timer			= &step_timer;
	stepper_init(&motor);

	uint8_t msg[MAX_SPI_TRANSFER_LENGTH];

	while(1)
	{
		queue_peek_blocking(&msgQueue, msg);

		stepper_msg_handle(&motor, msg);

		queue_remove_blocking(&msgQueue, msg);
	}
}

int main()
{
	stdio_init_all();

	gpio_init(PICO_PIN_SPI_MSG_REQ);
	gpio_set_dir(PICO_PIN_SPI_MSG_REQ, GPIO_IN);
	gpio_pull_down(PICO_PIN_SPI_MSG_REQ);
	gpio_set_irq_enabled_with_callback(	PICO_PIN_SPI_MSG_REQ,
										GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
										true,
										gpio_isr);

	gpio_init(PICO_PIN_SYNC_SIGNAL);
	gpio_set_dir(PICO_PIN_SYNC_SIGNAL, GPIO_IN);
	gpio_pull_down(PICO_PIN_SYNC_SIGNAL);
	gpio_set_irq_enabled_with_callback(	PICO_PIN_SYNC_SIGNAL,
										GPIO_IRQ_EDGE_RISE,
										true,
										gpio_isr);

	static uint spi_spinlock;
	spi_spinlock = spin_lock_claim_unused(true);
	queue_init_with_spinlock(	&spiQueue,
								sizeof(struct spi_client*),
								1,
								spi_spinlock);

	static uint msg_spinlock;
	msg_spinlock = spin_lock_claim_unused(true);
	queue_init_with_spinlock(	&msgQueue,
								MAX_SPI_TRANSFER_LENGTH,
								1,
								msg_spinlock);

	memset(spi_in_buf, 0, MAX_SPI_TRANSFER_LENGTH);
	memset(spi_out_buf, 0, MAX_SPI_TRANSFER_LENGTH);

	spi.pio		= pio0;
	spi.sm		= 0;
	spi.clk_div	= 1.f;
	spi.tr_pin	= PICO_PIN_SPI_MSG_READY;
	spi.in_pin	= PIO_SPI_DEFAULT_IN_PIN;
	spi.out_pin = PIO_SPI_DEFAULT_OUT_PIN;
	spi.bits	= PIO_SPI_BITS_PER_WORD;
	spi.words	= MAX_SPI_TRANSFER_LENGTH;
	spi.tx_buf	= spi_out_buf;
	spi.rx_buf	= spi_in_buf;

	pio_spi_client_init(&spi);

	multicore_launch_core1(core1_main);

	while(1)
		get_spi_msg();
}