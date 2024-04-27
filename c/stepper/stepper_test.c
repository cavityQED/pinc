#include "pi_stepper.h"

const static spi_mode = SPI_MODE_0;

static pincPiStepper		x_axis;	
static pincStepperConfig_t	x_config;
static pincStepperMove_t	x_move;
static pthread_mutex_t		spi_mutex;
static pthread_mutexattr_t	spi_attr;


void* config(void* data)
{
	stepper_config(&x_axis, &x_config);
}

void* move(void* data)
{
	stepper_move(&x_axis, &x_move);
}

int main(int argc, char const *argv[])
{
	gpioInitialise();

	x_config.axis		= X_AXIS;
	x_config.spmm		= X_SPMM;
	x_config.accel		= 0;			// no accel implementation on pico yet
	x_config.jog_steps	= X_SPMM;		// default jog - 1mm
	x_config.jog_speed	= 40 * X_SPMM;	// 40mm/s
	x_config.min_speed	= 10 * X_SPMM;	// 10mm/s
	x_config.max_speed	= 50 * X_SPMM;	// 50mm/s

	pthread_mutexattr_init(&spi_attr);
	pthread_mutexattr_settype(&spi_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_attr);
	
	int fd_CS0 = open("/dev/spidev0.0", O_RDWR);
	ioctl(fd_CS0, SPI_IOC_WR_MODE, &spi_mode);
	x_axis.fpga_spi_client.cs					= -1;
	x_axis.fpga_spi_client.fd					= fd_CS0;
	x_axis.fpga_spi_client.mutex				= &spi_mutex;

	memset(&(x_axis.fpga_spi_client.tr), 0, sizeof(spiTr));
	x_axis.fpga_spi_client.tr.speed_hz			= 500000;
	x_axis.fpga_spi_client.tr.delay_usecs		= 0;
	x_axis.fpga_spi_client.tr.bits_per_word		= 8;
	x_axis.fpga_spi_client.tr.cs_change			= 0;

	int fd_CS1 = open("/dev/spidev0.1", O_RDWR);
	ioctl(fd_CS1, SPI_IOC_WR_MODE, &spi_mode);
	x_axis.pico_spi_client.cs					= -1;
	x_axis.pico_spi_client.fd					= fd_CS1;
	x_axis.pico_spi_client.mutex				= &spi_mutex;

	memset(&x_axis.pico_spi_client.tr, 0, sizeof(spiTr));
	x_axis.pico_spi_client.tr.speed_hz			= 4000000;
	x_axis.pico_spi_client.tr.delay_usecs		= 0;
	x_axis.pico_spi_client.tr.bits_per_word		= 8;
	x_axis.pico_spi_client.tr.cs_change			= 0;

	pin_request_init(&x_axis.spi_request, X_SPI_HS_PIN, 1);

	x_axis.pin_status = X_STATUS_INT;
	gpioSetISRFuncEx(x_axis.pin_status, FALLING_EDGE, 0, stepper_pin_isr, &x_axis);

	pthread_t thread;
	pthread_create(&thread, NULL, config, NULL);
	pthread_join(thread, NULL);

	memset(&x_move, 0, sizeof(pincStepperMove_t));
	x_move.mode = JOG;
	x_move.end.x = X_SPMM;
	x_move.v_sps = x_config.jog_speed;

	pthread_create(&thread, NULL, move, NULL);
	pthread_join(thread, NULL);

	gpioTerminate();
	return 0;
}