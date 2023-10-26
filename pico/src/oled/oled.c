#include "oled.h"

static int seg_size(struct screen_segment* seg)
{
	return ((int)(seg->col_end - seg->col_start) + 1) * ((int)(seg->page_end - seg->page_start) + 1);
}

static void send_cmd(struct oled* screen, uint8_t cmd)
{
	uint8_t buf[2] = {0x00, cmd};
	i2c_write_blocking(i2c_default, screen->addr, buf, 2, false);
}

static void send_cmds(struct oled* screen, const uint8_t* cmds, int n)
{
	for(int i = 0; i < n; i++)
		send_cmd(screen, cmds[i]);
}

static void set_draw_segment(struct oled* screen, struct screen_segment* seg)
{
	uint8_t cmds[] = {	SET_COLUMN_ADDR_STR_END, seg->col_start, seg->col_end,
						SET_PAGE_STR_END, seg->page_start, seg->page_end};
	send_cmds(screen, cmds, count_of(cmds));
}

static void draw_segment(struct oled* screen, struct screen_segment* seg, const uint8_t* buf)
{
	set_draw_segment(screen, seg);
	
	int size = seg_size(seg);
	printf("Segment Size:\t%u bytes\n", size);

	uint8_t* tmp = malloc(size+1);

	tmp[0] = 0x40;
	memcpy(tmp + 1, buf, size);

	i2c_write_blocking(i2c_default, screen->addr, tmp, size+1, false);

	free(tmp);
}

static void draw_char(struct oled* screen, struct screen_segment* seg, char c)
{
	if(c >= '0' && c <= '9')
		c -= 45;
	else if(c == '.')
 		c = 2;
 	else if(c == '-')
 		c = 1;
 	else
 		c = 0;

 		int id = (int)c * 64;

 	draw_segment(screen, seg, &ascii_bytes[id]);
}

static void draw_string(struct oled* screen, char* str, int len)
{
 	struct screen_segment seg;
 	seg.col_start = 0x00;
 	seg.col_end = 0x0F;
 	seg.page_start = 0x00;
 	seg.page_end = 0x03;

 	for(int i = 0; i < len; i++)
 	{
 		draw_char(screen, &seg, str[i]);
 		seg.col_start += 0x0F;
 		seg.col_end += 0x0F;
 	}
}

static void clear_screen(struct oled* screen)
{
	uint size = screen->n_cols * screen->n_pages; 
	uint8_t* buf = malloc(size + 1);
	memset(buf, 0, size + 1);
}

static void draw_screen(struct oled* screen, const uint8_t* buf)
{
	struct screen_segment seg;
	seg.col_start	= 0x00;
	seg.page_start	= 0x00;
	seg.col_end		= screen->n_cols - 1;
	seg.page_end	= screen->n_pages - 1;

	draw_segment(screen, &seg, buf);
}