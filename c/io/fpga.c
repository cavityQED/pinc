#include "fpga_signal.h"
#include "stepper/stepper_common.h"

#define PIN_INT_0	27
#define PIN_INT_1	21
#define PIN_INT_2	13

static spiHost				spi_host;
static spiHost				spi_pico;
static spiMsg				spi_msg;


static fpga_signal			fpga;
static spiHost				spi_fpga;
static pthread_mutexattr_t	spi_attr;
static pthread_mutex_t		spi_mutex;





void send_pico_cmd(uint16_t cmd)
{
	static stepper_msg_t msg;
	msg.cmd = cmd;

	spi_write_msg(&spi_msg, &msg, sizeof(stepper_msg_t));

	spi_send(&spi_pico, &spi_msg);

	free(spi_msg.tx);
	free(spi_msg.rx);
	gpioWrite(6, 0);
}

void read_fpga(uint8_t	tx_byte)
{
	static uint8_t	tx[2];
	static uint8_t	rx[2];

	tx[0] = tx_byte;
	tx[1] = 0x00;

	spi_msg.tx = tx;
	spi_msg.rx = rx;
	spi_msg.len = 2;

	spi_send(&spi_fpga, &spi_msg);

	uint8_t val = spi_msg.rx[1];

	printf("FPGA Data:\tTX: 0x%2X\tRX: 0x%2X\n", spi_msg.tx[0], val);

	if(val == 0xF7)
		send_pico_cmd(STEPPER_CMD_UPDATE);
	if(val == 0x7B)
		gpioWrite(6, 1);
}
/*
int main()
{
	gpioInitialise();
	gpioSetMode(6, PI_OUTPUT);
	gpioWrite(6, 0);

	gpioSetMode(PIN_INT_0, PI_INPUT);
	gpioSetPullUpDown(PIN_INT_0, PI_PUD_UP);
	gpioSetMode(PIN_INT_1, PI_INPUT);
	gpioSetPullUpDown(PIN_INT_1, PI_PUD_UP);

	pthread_mutexattr_init(&spi_attr);
	pthread_mutexattr_settype(&spi_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_attr);

	spi_init(&spi_fpga, "/dev/spidev0.0", -1, 500000, 0, 8, 0, SPI_MODE_0, &spi_mutex);
	spi_init(&spi_pico, "/dev/spidev0.1", -1, 4000000, 0, 8, 0, SPI_MODE_0, &spi_mutex);

	// gpioWrite(6, 1);

	int xfers = 256;
	uint8_t	tx_byte = 0x00;
	while(xfers)
	{
		// read_fpga(tx_byte++);
		// --xfers;
		if(!gpioRead(PIN_INT_0) | !gpioRead(PIN_INT_1))
			read_fpga(tx_byte);
	}

}
*/

int main()
{
	gpioInitialise();

	pthread_mutexattr_init(&spi_attr);
	pthread_mutexattr_settype(&spi_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_attr);

	spi_init(&spi_fpga, "/dev/spidev0.0", -1, 500000, 0, 8, 0, SPI_MODE_0, &spi_mutex);

	fpga_sig_init(&fpga, PIN_INT_0, NULL, &spi_fpga, NULL);

	while(1)
	{
		gpioDelay(10000);
	}

}