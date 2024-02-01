#include "spi/host_spi.h"

typedef struct
{
	bool rd;
	uint8_t reg;
	uint8_t val;

} gyro_msg_t;

static spiHost		spi_host;
static spiMsg		spi_msg;

static gyro_msg_t	xlow;
static gyro_msg_t	xhigh;

static gyro_msg_t	ylow;
static gyro_msg_t	yhigh;

static gyro_msg_t	zlow;
static gyro_msg_t	zhigh;

static pthread_mutexattr_t	spi_attr;
static pthread_mutex_t		spi_mutex;

int main()
{
	gpioInitialise();

	pthread_mutexattr_init(&spi_attr);
	pthread_mutexattr_settype(&spi_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_attr);

	spi_init(&spi_host, "/dev/spidev0.0", -1, 500000, 0, 8, 0, SPI_MODE_0, &spi_mutex);

	uint8_t gyro_tx[3];
	uint8_t gyro_rx[3];

	spi_msg.tx = gyro_tx;
	spi_msg.rx = gyro_rx;
	spi_msg.len = 3;

	int16_t xdata;

	while(1)
	{
		xdata = 0x0000;
		memset(&gyro_tx, 0, 3);
		memset(&gyro_rx, 0, 3);

		gyro_tx[0] = 0x3B | 0x80;
		spi_send(&spi_host, &spi_msg);

		xdata |= (gyro_rx[1] << 8);
		xdata |= (gyro_rx[2]);

		// spi_send(&spi_host, &spi_msg);


		printf("X Accel Data:\t%d\n", xdata);
		gpioDelay(500000);
	}
}	
