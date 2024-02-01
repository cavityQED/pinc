#ifndef HOST_MAIN_H
#define HOST_MAIN_H

#include "c_pinc.h"

#define X_SPMM				800
#define X_PCF_PIN			12
#define X_FPGA_INTR			27
#define X_SPI_PIN			6
#define X_PCF_ADDR			0x20

#define Y_SPMM				800
#define Y_PCF_PIN			21
#define Y_SPI_PIN			17
#define Y_PCF_ADDR			0x21

#define SYNC_PIN			22
#define SPI_STATUS_READY	0x08
#define SYNC_STATUS_READY	0x10

#define INPUT_PCF_PIN		26
#define INPUT_PCF_ADDR		0x22

#define BUTTON_UP			0xDE
#define BUTTON_DOWN			0xDD
#define BUTTON_RIGHT		0xD7
#define BUTTON_LEFT			0xDB

#define BUTTON_BACK			0x0012
#define BUTTON_MENU			0x0022
#define BUTTON_STOP			0x0042
#define BUTTON_SELECT		0x0082

#define KNOB_CW				0x1400
#define KNOB_CCW			0x2400

typedef struct
{
	uint8_t		axis;
	uint32_t	sps;
	int			steps;
	
} jog_mode_config_t;

static stepper_t			xaxis;
static stepper_t			yaxis;
static PCF					input;
static fpga_signal			fpga_input;
static spiHost				spi_host;
static spiHost				spi_fpga;
static request_t			spi_base_req;
static request_t			sync_base_req;
static lcd_font_t			lcd_font;
static jog_mode_config_t	jog_mode;
static Tree					request_tree;

pthread_mutex_t		spi_mutex;
pthread_mutexattr_t	spi_attr;


#endif