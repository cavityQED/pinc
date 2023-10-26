#include "st7735.h"

static spiHost	spi_lcd;
static uint8_t	lcd_init_list[] =
	{
		SOFT_RESET, 0,
		DISPLAY_OFF, 0,
		MEMORY_ACCESS_CONTROL, 1, RGB_ORDER,
		PIXEL_FORMAT, 1, 0x55,

		NORMAL_DISPLAY_MODE_ON, 0,
		SLEEP_OUT, WAIT,
		DISPLAY_ON, WAIT,
		NOP
	};

void lcd_init(pthread_mutex_t* mutex)
{
	lcd_reset();
	spi_init(&spi_lcd, "/dev/spidev0.1", PIN_CS, 16000000, 0, 8, 0, SPI_MODE_0, mutex);
	lcd_send_cmd_list(lcd_init_list);
}

void lcd_reset()
{
	gpioWrite(PIN_RESET, 1);
	gpioDelay(50000);
	gpioWrite(PIN_RESET, 0);
	gpioDelay(50000);
	gpioWrite(PIN_RESET, 1);
	gpioDelay(50000);
}

void lcd_set_window(lcdSegment* seg)
{
	uint8_t data[] = 
	{
		COLUMN_ADDRESS_SET, 4, seg->x >> 8, seg->x & 0xFF, (seg->x + seg->w - 1) >> 8, (seg->x + seg->w - 1) & 0xFF,
		PAGE_ADDRESS_SET, 4, seg->y >> 8, seg->y & 0xFF, (seg->y + seg->h - 1) >> 8, (seg->y + seg->h - 1) & 0xFF,
		MEMORY_WRITE, 0,
		NOP
	};

	lcd_send_cmd_list(data);
}

void lcd_draw_segment(lcdSegment* seg)
{
	lcd_set_window(seg);
	lcd_send_bytes(seg->data, seg->w * seg->h * sizeof(seg->on_clr));
}

void lcd_draw_string(lcdSegment* seg, lcd_font_t* font, char* str)
{
	seg->data = malloc(2*font->w*font->h);
	
	int n = strlen(str);
	for(int i = 0; i < n; i++)
	{
		lcd_write_segment_char_from_font(seg, font, str[i]);
		lcd_draw_segment(seg);
		seg->x = seg->x + font->w;
	}

	free(seg->data);
}






void lcd_send_bytes(uint8_t* data, uint32_t n)
{
	spiMsg msg;
	msg.tx = malloc(MAX_SPI_TRANSFER_LENGTH);
	msg.rx = malloc(MAX_SPI_TRANSFER_LENGTH);

	gpioWrite(PIN_DCX, 1);

	uint32_t id = 0;
	while(n > 0)
	{
		if(n < MAX_SPI_TRANSFER_LENGTH)
		{
			memcpy(msg.tx, &data[id], n);
			msg.len = n;
			n = 0;
		}
		else
		{
			memcpy(msg.tx, &data[id], MAX_SPI_TRANSFER_LENGTH);
			msg.len = MAX_SPI_TRANSFER_LENGTH;
			n -= MAX_SPI_TRANSFER_LENGTH;
			id += MAX_SPI_TRANSFER_LENGTH;
		}

		spi_send(&spi_lcd, &msg);
	}

	free(msg.tx);
	free(msg.rx);
}

void lcd_send_cmd(uint8_t* data, spiMsg* msg)
{
	msg->len	= 1;
	msg->tx[0]	= *data++;
	gpioWrite(PIN_DCX, 0);
	spi_send(&spi_lcd, msg);

	uint8_t n = *data++;
	uint8_t len = n & 0x7F;
	if(len)
	{
		memcpy(msg->tx, data, len);
		msg->len = len;
		gpioWrite(PIN_DCX, 1);
		spi_send(&spi_lcd, msg);
	}

	if(n & WAIT)
		gpioDelay(120000);
}

void lcd_send_cmd_list(uint8_t* list)
{
	spiMsg msg;
	msg.tx		= malloc(SPI_TRANSFER_LENGTH);
	msg.rx		= malloc(SPI_TRANSFER_LENGTH);

	while(*list != NOP)
	{
		lcd_send_cmd(list++, &msg);
		list += (1 + (*list & 0x7F)); 
	}

	free(msg.tx);
	free(msg.rx);
}


void lcd_write_screen_segment(lcdScreen* scr, lcdSegment* seg)
{
	uint32_t idx;
	uint32_t idy;
	uint32_t width = scr->fs_data->w;

	for(int row = 0; row < seg->w; row++)
	{
		idy = seg->y + row;
		for(int col = 0; col < seg->h; col++)
		{
			idx = seg->x + col;
			scr->fs_data->data[idy * width + idx] = seg->data[col * seg->w + row];
		}
	}
}

void lcd_write_pxl(uint8_t* pxl, uint16_t color)
{
	*pxl++ = color >> 8;
	*pxl = color & 0xFF;
}

void lcd_write_segment_row_byte(lcdSegment* seg, uint16_t x, uint16_t y, uint8_t mask)
{
	uint32_t id;
	uint16_t color;
	for(int b = 0; b < 8; b++)
	{
		id = 2*(y*seg->w+x+b);
		color = (mask & 0x80)? seg->on_clr : seg->off_clr;
		lcd_write_pxl(&seg->data[id], color);
		mask <<= 1;
	}
}

void lcd_write_segment_bit_masks(lcdSegment* seg, const uint8_t* bit_masks)
{
	for(int row = 0; row < seg->h; row++)
	{
		for(int col = 0; col < seg->w / 8; col += 8)
			lcd_write_segment_row_byte(seg, col, row, *bit_masks++);
	}
}

void lcd_write_segment_char(lcdSegment* seg, char c)
{
	if(c <= '9' && c >= '0')
		c -= 45;
	else
		c = 0;

	uint32_t id = c * seg->w * seg->h / 8;

	lcd_write_segment_bit_masks(seg, &ascii_bytes[id]);
}

void lcd_fill_segment_color(lcdSegment* seg, uint16_t color)
{
	uint32_t n = seg->w*seg->h;
	uint8_t* pxl = seg->data;
	while(n-- > 0)
	{
		lcd_write_pxl(pxl, color);
		pxl += 2;
	}
}