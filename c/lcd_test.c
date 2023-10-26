#include "lcd/st7735.h"
#include "lcd/ascii_char_bit_maps.h"


static const uint16_t x_start = 2;
static const uint16_t y_start = 1;
static const uint16_t width = 130;
static const uint16_t height = 130;
static const uint16_t line_width = 2;





static void draw_border()
{
	lcdSegment seg;
	seg.x = x_start;
	seg.y = y_start;
	seg.w = width;
	seg.h = line_width;
	seg.data = malloc(seg.w*seg.h*sizeof(seg.on_clr));
	lcd_fill_segment_color(&seg, 0xFFFF);
	lcd_draw_segment(&seg);


	seg.y += 34;
	lcd_draw_segment(&seg);

	seg.y += 32;
	lcd_draw_segment(&seg);

	seg.y += 32;
	lcd_draw_segment(&seg);

	seg.y += 32;
	lcd_draw_segment(&seg);

	seg.y += 32;
	lcd_draw_segment(&seg);

	seg.y += 32;
	lcd_draw_segment(&seg);

	seg.y = height - line_width;
	lcd_draw_segment(&seg);
	free(seg.data);

	seg.y = 0;
	seg.w = line_width;
	seg.h = height;
	seg.data = malloc(seg.w*seg.h*sizeof(seg.on_clr));
	lcd_fill_segment_color(&seg, 0xFFFF);
	lcd_draw_segment(&seg);

	seg.x = width - line_width;
	lcd_draw_segment(&seg);
	free(seg.data);
}

void draw_string(char* str, lcd_font_t* font, uint16_t x, uint16_t y)
{
	lcdSegment seg;
	seg.x = x;
	seg.y = y;
	seg.on_clr = ST7735_GREEN;
	seg.off_clr = ST7735_BLUE;
	seg.data = malloc(2*font->w*font->h);
	int n = strlen(str);
	for(int i = 0; i < n; i++)
	{
		lcd_write_segment_char_from_font(&seg, font, str[i]);
		lcd_draw_segment(&seg);
		x = x + font->w;
		seg.x = x;
	}

	free(seg.data);
}

static pthread_mutex_t spi_mutex = PTHREAD_MUTEX_INITIALIZER;
int main()
{
	gpioInitialise();
	lcd_init(&spi_mutex);

	lcdSegment seg;
	seg.x = 0;
	seg.y = 0;
	seg.w = width;
	seg.h = height;
	seg.data = malloc(seg.w * seg.h * sizeof(seg.on_clr));

	lcd_fill_segment_color(&seg, ST7735_BLUE);
	lcd_draw_segment(&seg);
	free(seg.data);

	lcd_font_t font;
	font.w = 8;
	font.h = 12;
	font.n = 95;
	font.offset = 32;
	font.data = ascii_font_8x12;

	draw_border();

	draw_string("abc123", &font, 8, 4);
	draw_string("X: -12345.67890", &font, 8, 20);

}