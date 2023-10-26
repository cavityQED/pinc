#ifndef LCD_H
#define LCD_H

#include "st7735.h"

typedef struct
{
	uint8_t	w;		// width (pixels/bits)
	uint8_t h;		// height (pixels/bits)
	uint8_t	n;		// number of characters
	uint8_t	offset;	// offset from ascii start to font start

	uint8_t* data;

} lcd_font_t;

static inline uint8_t* lcd_get_char_bitmap(lcd_font_t* font, char c)
{
	uint8_t id = (int)c - font->offset;
	return &font->data[id];
}

#endif