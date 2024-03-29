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
	static stepper_msg_t msg;
	switch(pin)
	{
		case PIN_SPI_MSG_REQUEST:
			if(event_mask & GPIO_IRQ_EDGE_RISE)
				queue_try_add(&spiQueue, &spi);
			else if(event_mask & GPIO_IRQ_EDGE_FALL)
			{	
				// spi transfer complete
				pio_spi_client_disable(&spi);
				memcpy(&msg, (stepper_msg_t*)spi.rx_buf, sizeof(stepper_msg_t));
				queue_try_add(&msgQueue, &msg);
			}
			break;

		case PIN_SYNC_SIGNAL:
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

	stepper_info_t info;
	info.status = motor.status;
	info.pos = motor.pos;
	memset(spi_in_buf, 0, SPI_TRANSFER_LENGTH);
	memset(spi_out_buf, 0, SPI_TRANSFER_LENGTH);
	memcpy(spi_out_buf, (unsigned long*)&info, sizeof(stepper_info_t));
	spi.tx_buf = spi_out_buf;

	pio_spi_client_transfer_blocking(&spi);
	
	queue_remove_blocking(&spiQueue, &spi);
}

static void core1_main()
{
	motor.status		= MOVE_READY;
	motor.p_step		= PICO_STEP_PIN;
	motor.p_dir			= PICO_DIR_PIN;
	motor.p_ena			= PICO_ENA_PIN;
	motor.p_motion		= PIN_MOTION;
	motor.spmm			= 800;
	motor.delay			= 125;
	motor.alarmPool		= alarm_pool_create(1, 2);
	motor.timer			= &step_timer;
	stepper_init(&motor);

	static stepper_msg_t msg;

	while(1)
	{
		queue_peek_blocking(&msgQueue, &msg);

		stepper_msg_handle(&motor, &msg);

		queue_remove_blocking(&msgQueue, &msg);
	}
}

int main()
{
	stdio_init_all();

	gpio_init(PIN_SPI_MSG_REQUEST);
	gpio_set_dir(PIN_SPI_MSG_REQUEST, GPIO_IN);
	gpio_pull_down(PIN_SPI_MSG_REQUEST);
	gpio_set_irq_enabled_with_callback(	PIN_SPI_MSG_REQUEST,
										GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
										true,
										gpio_isr);

	gpio_init(PIN_SYNC_SIGNAL);
	gpio_set_dir(PIN_SYNC_SIGNAL, GPIO_IN);
	gpio_pull_down(PIN_SYNC_SIGNAL);
	gpio_set_irq_enabled_with_callback(	PIN_SYNC_SIGNAL,
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
								sizeof(stepper_msg_t),
								1,
								msg_spinlock);

	for(uint8_t i = SPI_TRANSFER_LENGTH; i > 0; i--)
		spi_out_buf[i-1] = i;
	spi_out_buf[0] = 0x80;

	spi.pio		= pio0;
	spi.sm		= 0;
	spi.clk_div	= 1.f;
	spi.in_pin	= PIO_SPI_DEFAULT_IN_PIN;
	spi.out_pin = PIO_SPI_DEFAULT_OUT_PIN;
	spi.tr_pin	= PIN_SPI_MSG_READY;
	spi.bits	= PIO_SPI_BITS_PER_WORD;
	spi.words	= SPI_TRANSFER_LENGTH;
	spi.tx_buf	= spi_out_buf;
	spi.rx_buf	= spi_in_buf;

	pio_spi_client_init(&spi);

	multicore_launch_core1(core1_main);

	while(1)
		get_spi_msg();
}