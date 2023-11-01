#ifndef ST7735_H
#define ST7735_H

#include "pigpio.h"

#include "spi/host_spi.h"
#include "host_common.h"
#include "ascii_char_bit_maps.h"

#define WAIT						0x80
#define NOP							0x00
#define SOFT_RESET					0x01

#define SLEEP_IN					0x10
#define SLEEP_OUT					0x11
#define PARTIAL_MODE_ON				0x12
#define NORMAL_DISPLAY_MODE_ON		0x13

#define DISPLAY_INVERSION_OFF		0x20
#define DISPLAY_INVERSION_ON		0x21
#define ALL_PIXEL_OFF				0x22
#define ALL_PIXEL_ON				0x23
#define DISPLAY_OFF					0x28
#define DISPLAY_ON					0x29
#define PARTIAL_ADDRESS_SET			0x30

#define MEMORY_ACCESS_CONTROL		0x36
	#define ROW_ORDER				0x80
	#define COL_ORDER				0x40
	#define ROW_COL_EXCHANGE		0x20
	#define V_REFRESH				0x10
	#define RGB_ORDER				0x08
	#define H_REFRESH				0x04

#define COLUMN_ADDRESS_SET			0x2A
#define PAGE_ADDRESS_SET			0x2B
#define MEMORY_WRITE				0x2C

#define IDLE_MODE_OFF				0x38
#define IDLE_MODE_ON				0x39

#define PIXEL_FORMAT				0x3A
	#define PIXEL_3_BIT_RGB			0x01
	#define PIXEL_16_BIT_RGB		0x05

#define MEMORY_WRITE_CONTINUE		0x3C
#define BRIGHTNESS_SET				0x51
#define WRITE_CTRL_DISPLAY_VALUE	0x53

#define FRAME_CONTROL_1				0xB1
#define FRAME_CONTROL_2				0xB2
#define FRAME_CONTROL_3				0xB3
#define DISPLAY_INVERSION_CONTROL	0xB4
#define DISPLAY_FUNCTION_CONTROL	0xB6

#define POWER_CONTROL_1				0xC0	// set GVDD
#define POWER_CONTROL_2				0xC1	// set VGH/VGL
#define POWER_CONTROL_3				0xC2	// op-amp adjust nomarl mode
#define POWER_CONTROL_4				0xC3	// op-amp adjust idle mode
#define POWER_CONTROL_5				0xC4	// op-amp adjust partial mode
#define POWER_CONTROL_6				0xFC	// partial mode and idle
#define	VCOM_CONTROL				0xC5	// set VCOMH/VCOML
#define VCOM_OFFSET					0xC7
#define VCOM_CONTROL_4				0xFF

#define POS_GAMMA_CONTROL			0xE0
#define NEG_GAMMA_CONTROL			0xE1

#define PIN_DCX		20
#define PIN_RESET	16
#define PIN_CS		13

#define ST7735_BLACK	0x0000
#define ST7735_BLUE		0x137F
#define ST7735_RED		0xF800
#define ST7735_GREEN	0x07E0
#define ST7735_CYAN		0x07FF
#define ST7735_MAGENTA	0xF81F
#define ST7735_YELLOW	0xFFE0
#define ST7735_WHITE	0xFFFF
#define ST7735_TAN		0xED01
#define ST7735_GREY		0x9CD1
#define ST7735_BROWN	0x6201
#define ST7735_DGREEN	0x01c0
#define ST7735_ORANGE	0xFC00


typedef struct
{
	uint8_t	w;		// width (pixels/bits)
	uint8_t h;		// height (pixels/bits)
	uint8_t	n;		// number of characters
	uint8_t	offset;	// offset from ascii start to font start

	uint8_t* data;

} lcd_font_t;

typedef struct lcd_segment_t
{
	uint16_t	x;
	uint16_t	y;
	uint16_t	w;
	uint16_t	h;

	uint8_t*	data;

	uint16_t on_clr;
	uint16_t off_clr;

} lcdSegment;

typedef struct screen_t
{
	int		p_cs;
	int		p_dc;

	spiMsg		spi_msg;
	lcdSegment*	fs_data;	// full screen data

} lcdScreen;

typedef struct
{
	uint16_t x;
	uint16_t y;

	uint16_t on_clr;
	uint16_t off_clr;

	bool on;

} pxl_t;

void lcd_reset();
void lcd_init(pthread_mutex_t* mutex);
void lcd_set_window(lcdSegment* seg);

void lcd_send_cmd(uint8_t* data, spiMsg* msg);
void lcd_send_cmd_list(uint8_t* list);
void lcd_send_bytes(uint8_t* data, uint32_t n);

void lcd_draw_string(lcdSegment* seg, lcd_font_t* font, char* str);
void lcd_draw_segment(lcdSegment* seg);
void lcd_write_segment_col_byte(lcdSegment* seg, uint16_t x, uint16_t y, uint8_t mask);
void lcd_write_segment_row_byte(lcdSegment* seg, uint16_t x, uint16_t y, uint8_t mask);
void lcd_write_segment_bit_masks(lcdSegment* seg, const uint8_t* bit_masks);
void lcd_write_segment_char(lcdSegment* seg, char c);
void lcd_fill_segment_color(lcdSegment* seg, uint16_t color);

static inline uint8_t* lcd_get_char_bitmap(lcd_font_t* font, char c)
{
	uint32_t id = (c - font->offset)*(font->w * font->h / 8);
	return &font->data[id];
}

static inline void lcd_write_segment_char_from_font(lcdSegment* seg, lcd_font_t* font, char c)
{
	seg->w = font->w;
	seg->h = font->h;
	lcd_write_segment_bit_masks(seg, lcd_get_char_bitmap(font, c));
}


#endif