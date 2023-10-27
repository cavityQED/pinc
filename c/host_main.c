#include "host_main.h"

void move(jog_mode_config_t* jog)
{
	static stepper_move_t m;
	memset(&m, 0, sizeof(stepper_move_t));

	m.mode	= LINE_MODE;
	m.sps	= jog->sps;
	m.end.x = 0;
	m.end.y = 0;

	if(jog->axis == X_AXIS)
	{
		m.end.x = (jog->dir)? xaxis.config.spmm : -xaxis.config.spmm;
		stepper_move(&xaxis, &m);
	}
	else if(jog->axis == Y_AXIS)
	{
		m.end.y = (jog->dir)? yaxis.config.spmm : -yaxis.config.spmm;
		stepper_move(&yaxis, &m);
	}
}

void input_isr(int gpio, int level, uint32_t tick, void* dev)
{
	PCF* pcf = (PCF*)dev;

	pcf_get_signal(pcf);

	uint16_t mask = (pcf->sig.high << 8) | pcf->sig.low;
	switch(mask)
	{
		case BUTTON_RIGHT:
			jog_mode.axis = X_AXIS;
			printf("Right Button\n");
			break;
		case BUTTON_LEFT:
			jog_mode.axis = Y_AXIS;
			printf("Left Button\n");
			break;
		case BUTTON_UP:
			printf("Up Button\n");
			break;
		case BUTTON_DOWN:
			printf("Down Button\n");
			break;
		case BUTTON_MENU:
			printf("Menu Button\n");
			break;
		case BUTTON_BACK:
			printf("Back Button\n");
			break;
		case BUTTON_SELECT:
			printf("Select Button\n");
			stepper_lcd_draw_pos(&xaxis);
			break;
		case BUTTON_STOP:
			printf("Stop Button\n");
			break;
		case KNOB_CW:
			jog_mode.dir = 1;
			printf("Knob CW\n");
			move(&jog_mode);
			break;
		case KNOB_CCW:
			jog_mode.dir = 0;
			printf("Knob CCW\n");
			move(&jog_mode);
			break;

		default:
			break;
	}
}

int main()
{
	gpioInitialise();

	pthread_mutexattr_init(&spi_attr);
	pthread_mutexattr_settype(&spi_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_attr);

	spi_init(&spi_host, "/dev/spidev0.0", -1, 4000000, 0, 8, 0, SPI_MODE_0, &spi_mutex);

	pcf_init(&input, INPUT_PCF_ADDR, INPUT_PCF_PIN, NULL, input_isr);
	pcf_init(&xaxis.pcf_sig, X_PCF_ADDR, X_PCF_PIN, &xaxis, stepper_status_isr);
	pcf_init(&yaxis.pcf_sig, Y_PCF_ADDR, Y_PCF_PIN, &yaxis, stepper_status_isr);

	request_init(&spi_base_req, 0, SPI_STATUS_READY);
	request_init(&sync_base_req, 0, SYNC_STATUS_READY);

	pin_request_init(&xaxis.spi_req, &spi_base_req, X_SPI_PIN, 1);
	pin_request_init(&yaxis.spi_req, &spi_base_req, Y_SPI_PIN, 1);

	lcd_font.w = 8;
	lcd_font.h = 12;
	lcd_font.n = 95;
	lcd_font.offset = 32;
	lcd_font.data = ascii_font_8x12;

	stepper_config_t		xconfig;
	xconfig.axis			= X_AXIS;
	xconfig.spmm			= X_SPMM;
	xconfig.accel			= 40 * X_SPMM;
	xconfig.jog_speed		= 40 * X_SPMM;
	xconfig.min_speed		= 10 * X_SPMM;
	xconfig.max_speed		= 50 * X_SPMM;
	xaxis.spi_msg.len		= SPI_TRANSFER_LENGTH;
	xaxis.spi_host			= &spi_host;
	xaxis.lcd_font			= &lcd_font;
	xaxis.lcd_seg.x			= 8;
	xaxis.lcd_seg.y			= 24;
	xaxis.lcd_seg.on_clr	= ST7735_BLACK;
	xaxis.lcd_seg.off_clr	= ST7735_BLUE;

	stepper_config_t		yconfig;
	yconfig.axis			= Y_AXIS;
	yconfig.spmm			= Y_SPMM;
	yconfig.accel			= 40 * Y_SPMM;
	yconfig.jog_speed		= 40 * Y_SPMM;
	yconfig.min_speed		= 10 * Y_SPMM;
	yconfig.max_speed		= 50 * Y_SPMM;
	yaxis.spi_msg.len		= SPI_TRANSFER_LENGTH;
	yaxis.spi_host			= &spi_host;
	yaxis.lcd_font			= &lcd_font;
	yaxis.lcd_seg.x			= 8;
	yaxis.lcd_seg.y			= 36;
	yaxis.lcd_seg.on_clr	= ST7735_BLACK;
	yaxis.lcd_seg.off_clr	= ST7735_BLUE;

	stepper_init(&xaxis, &xconfig);
	stepper_init(&yaxis, &yconfig);

	jog_mode.sps = 40 * X_SPMM;

	lcd_init(&spi_mutex);

	lcdSegment seg;
	seg.x = 0;
	seg.y = 0;
	seg.w = 130;
	seg.h = 130;
	seg.data = malloc(2*seg.w*seg.h);
	lcd_fill_segment_color(&seg, ST7735_BLUE);
	lcd_draw_segment(&seg);

	free(seg.data);

	while(1);

}
