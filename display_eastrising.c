#include "display.h"
#include "display_eastrising.h"
#include "i2c.h"
#include "config.h"
#include "sys_time.h"

#include <stdint.h>
#include <stdbool.h>

enum
{
	mapeof = 0xffffffff,
};

enum
{
	reg_pwrr =		0x01,
	reg_mrwc =		0x02,
	reg_pcsr =		0x04,
	reg_sysr =		0x10,
	reg_hdwr =		0x14,
	reg_hndftr =	0x15,
	reg_hndr =		0x16,
	reg_hstr =		0x17,
	reg_hpwr =		0x18,
	reg_vdhr0 =		0x19,
	reg_vdhr1 =		0x1a,
	reg_vndr0 =		0x1b,
	reg_vndr1 =		0x1c,
	reg_vstr0 =		0x1d,
	reg_vstr1 =		0x1e,
	reg_vpwr =		0x1f,
	reg_dpcr =		0x20,
	reg_fncr0 =		0x21,
	reg_fncr1 =		0x22,
	reg_fldr =		0x29,
	reg_curxl =		0x2a,
	reg_curxh =		0x2b,
	reg_curyl =		0x2c,
	reg_curyh =		0x2d,
	reg_fncr2 =		0x2e,
	reg_hsaw0 =		0x30,
	reg_hsaw1 =		0x31,
	reg_vsaw0 =		0x32,
	reg_vsaw1 =		0x33,
	reg_heaw0 =		0x34,
	reg_heaw1 =		0x35,
	reg_veaw0 =		0x36,
	reg_veaw1 =		0x37,
	reg_mwcr0 =		0x40,
	reg_mwcr1 =		0x41,
	reg_ltpr0 =		0x52,
	reg_ltpr1 =		0x53,
	reg_bgcr0 =		0x60,
	reg_bgcr1 =		0x61,
	reg_bgcr2 =		0x62,
	reg_fgcr0 =		0x63,
	reg_fgcr1 =		0x64,
	reg_fgcr2 =		0x65,
	reg_bgtr0 =		0x67,
	reg_bgtr1 =		0x68,
	reg_bgtr2 =		0x69,
	reg_p1cr =		0x8a,
	reg_p1dcr =		0x8b,
	reg_mclr =		0x8e,
	reg_pll_c1 =	0x88,
	reg_pll_c2 =	0x89,

	reg_sysr_color_depth_8 =				0b00000000,
	reg_sysr_color_depth_16 =				0b00001000,
	reg_sysr_if_8bit =						0b00000000,
	reg_sysr_if_16bit =						0b00000010,

	reg_pcsr_sample_rising_edge =			0b00000000,
	reg_pcsr_sample_falling_edge =			0b10000000,
	reg_pcsr_clock_period_system =			0b00000000,
	reg_pcsr_clock_period_system_by_2 =		0b00000001,
	reg_pcsr_clock_period_system_by_4 =		0b00000010,
	reg_pcsr_clock_period_system_by_8 =		0b00000011,

	reg_hndftr_de_polarity_active_high =	0b00000000,
	reg_hndftr_de_polarity_active_low =		0b10000000,

	reg_hpwr_hsync_polarity_active_low = 	0b00000000,
	reg_hpwr_hsync_polarity_active_high = 	0b10000000,

	reg_vpwr_vsync_polarity_active_low = 	0b00000000,
	reg_vpwr_vsync_polarity_active_high = 	0b10000000,

	reg_p1cr_pwm1_enable =					0b10000000,
	reg_p1cr_disable_level_low =			0b00000000,
	reg_p1cr_disable_level_high =			0b01000000,
	reg_p1cr_function_pwm1 =				0b00000000,
	reg_p1cr_function_fixed =				0b00010000,
	reg_p1cr_clock_ratio_1 =				0b00000000,
	reg_p1cr_clock_ratio_2 =				0b00000001,
	reg_p1cr_clock_ratio_4 =				0b00000010,
	reg_p1cr_clock_ratio_8 =				0b00000011,
	reg_p1cr_clock_ratio_16 =				0b00000100,
	reg_p1cr_clock_ratio_32 =				0b00000101,
	reg_p1cr_clock_ratio_64 =				0b00000110,
	reg_p1cr_clock_ratio_128 =				0b00000111,
	reg_p1cr_clock_ratio_256 =				0b00001000,
	reg_p1cr_clock_ratio_512 =				0b00001001,
	reg_p1cr_clock_ratio_1024 =				0b00001010,
	reg_p1cr_clock_ratio_2048 =				0b00001011,
	reg_p1cr_clock_ratio_4096 =				0b00001100,
	reg_p1cr_clock_ratio_8192 =				0b00001101,
	reg_p1cr_clock_ratio_16384 =			0b00001110,
	reg_p1cr_clock_ratio_32768 =			0b00001111,

	reg_pwrr_display_enable =				0b10000000,
	reg_pwrr_display_disable =				0b00000000,
	reg_pwrr_display_sleep_mode_enable =	0b00000010,
	reg_pwrr_display_sleep_mode_disable =	0b00000000,
	reg_pwrr_display_reset_start =			0b00000001,
	reg_pwrr_display_reset_complete =		0b00000000,

	reg_mwcr1_graphic_cursor_disable =		0b00000000,
	reg_mwcr1_graphic_cursor_enable =		0b10000000,
	reg_mwcr1_graphic_cursor_select_0 =		0b00000000,
	reg_mwcr1_graphic_cursor_select_1 =		0b00010000,
	reg_mwcr1_graphic_cursor_select_2 =		0b00100000,
	reg_mwcr1_graphic_cursor_select_3 =		0b00110000,
	reg_mwcr1_graphic_cursor_select_4 =		0b01000000,
	reg_mwcr1_graphic_cursor_select_5 =		0b01010000,
	reg_mwcr1_graphic_cursor_select_6 =		0b01100000,
	reg_mwcr1_graphic_cursor_select_7 =		0b01110000,
	reg_mwcr1_write_destination_layer =		0b00000000,
	reg_mwcr1_write_destination_cgram =		0b00000100,
	reg_mwcr1_write_destination_cursor =	0b00001000,
	reg_mwcr1_write_destination_pattern =	0b00001100,
	reg_mwcr1_write_destination_layer_0 =	0b00000000,
	reg_mwcr1_write_destination_layer_1 =	0b00000001,

	reg_mclr_memory_clear_start =			0b10000000,
	reg_mclr_memory_clear_complete =		0b00000000,
	reg_mclr_memory_area_active_window =	0b01000000,
	reg_mclr_memory_area_full_window =		0b00000000,

	reg_mwcr0_mode_text =					0b10000000,
	reg_mwcr0_mode_graphic =				0b00000000,
	reg_mwcr0_cursor_visible =				0b01000000,
	reg_mwcr0_cursor_invisible =			0b00000000,
	reg_mwcr0_cursor_blink =				0b00100000,
	reg_mwcr0_cursor_steady =				0b00000000,
	reg_mwcr0_memory_write_direction_lrtd =	0b00000000,
	reg_mwcr0_memory_write_direction_rltd =	0b00000100,
	reg_mwcr0_memory_write_direction_tdlr =	0b00001000,
	reg_mwcr0_memory_write_direction_dtlr =	0b00001100,
	reg_mwcr0_memory_write_autoincr_en =	0b00000000,
	reg_mwcr0_memory_write_autoincr_dis =	0b00000010,
	reg_mwcr0_memory_read_autoincr_en =		0b00000000,
	reg_mwcr0_memory_read_autoincr_dis =	0b00000001,

	reg_fncr0_font_cgrom =					0b00000000,
	reg_fncr0_font_cgram =					0b10000000,
	reg_fncr0_font_internal =				0b00000000,
	reg_fncr0_font_external =				0b00100000,
	reg_fncr0_encoding_8859_1 =				0b00000000,
	reg_fncr0_encoding_8859_2 =				0b00000001,
	reg_fncr0_encoding_8859_3 =				0b00000010,
	reg_fncr0_encoding_8859_4 =				0b00000011,

	reg_fncr1_font_align_disable =			0b00000000,
	reg_fncr1_font_align_enable =			0b10000000,
	reg_fncr1_font_transparent =			0b01000000,
	reg_fncr1_font_opaque =					0b00000000,
	reg_fncr1_font_straight =				0b00000000,
	reg_fncr1_font_rotate_90 =				0b00010000,
	reg_fncr1_font_enlarge_hor_x1 =			0b00000000,
	reg_fncr1_font_enlarge_hor_x2 =			0b00000100,
	reg_fncr1_font_enlarge_hor_x3 =			0b00001000,
	reg_fncr1_font_enlarge_hor_x4 =			0b00001100,
	reg_fncr1_font_enlarge_ver_x1 =			0b00000000,
	reg_fncr1_font_enlarge_ver_x2 =			0b00000001,
	reg_fncr1_font_enlarge_ver_x3 =			0b00000010,
	reg_fncr1_font_enlarge_ver_x4 =			0b00000011,

	reg_fncr2_font_size_16x16 =				0b00000000,
	reg_fncr2_font_size_24x24 =				0b01000000,
	reg_fncr2_font_size_32x32 =				0b10000000,

	reg_ltpr0_scroll_both =					0b00000000,
	reg_ltpr0_scroll_layer_1 =				0b01000000,
	reg_ltpr0_scroll_layer_2 =				0b10000000,
	reg_ltpr0_scroll_buffer =				0b11000000,
	reg_ltpr0_floatwin_transparency_dis =	0b00000000,
	reg_ltpr0_floatwin_transparency_en =	0b00100000,
	reg_ltpr0_visible_layer_1 =				0b00000000,
	reg_ltpr0_visible_layer_2 =				0b00000001,
	reg_ltpr0_visible_layer_lighten =		0b00000010,
	reg_ltpr0_visible_layer_transparent =	0b00000011,
	reg_ltpr0_visible_layer_or =			0b00000100,
	reg_ltpr0_visible_layer_and =			0b00000101,
	reg_ltpr0_visible_layer_floatwin =		0b00000110,
	reg_ltpr0_visible_layer_unused =		0b00000111,

	reg_ltpr1_transparency_layer_2_8_8 =	0b00000000,
	reg_ltpr1_transparency_layer_2_7_8 =	0b00010000,
	reg_ltpr1_transparency_layer_2_6_8 =	0b00100000,
	reg_ltpr1_transparency_layer_2_5_8 =	0b00110000,
	reg_ltpr1_transparency_layer_2_4_8 =	0b01000000,
	reg_ltpr1_transparency_layer_2_3_8 =	0b01010000,
	reg_ltpr1_transparency_layer_2_2_8 =	0b01100000,
	reg_ltpr1_transparency_layer_2_1_8 =	0b01110000,
	reg_ltpr1_transparency_layer_2_0_8 =	0b10000000,
	reg_ltpr1_transparency_layer_1_8_8 =	0b00000000,
	reg_ltpr1_transparency_layer_1_7_8 =	0b00000001,
	reg_ltpr1_transparency_layer_1_6_8 =	0b00000010,
	reg_ltpr1_transparency_layer_1_5_8 =	0b00000011,
	reg_ltpr1_transparency_layer_1_4_8 =	0b00000100,
	reg_ltpr1_transparency_layer_1_3_8 =	0b00000101,
	reg_ltpr1_transparency_layer_1_2_8 =	0b00000110,
	reg_ltpr1_transparency_layer_1_1_8 =	0b00000111,
	reg_ltpr1_transparency_layer_1_0_8 =	0b00001000,

	reg_dpcr_one_layer =					0b00000000,
	reg_dpcr_two_layer =					0b10000000,
	reg_dpcr_hor_scan_ltor =				0b00000000,
	reg_dpcr_hor_scan_rtol =				0b00001000,
	reg_dpcr_vert_scan_ltor =				0b00000000,
	reg_dpcr_vert_scan_rtol =				0b00000100,

	display_width = 480,
	display_height = 272,
	display_horizontal_blanking = 38,
	display_horizontal_blanking_fine = 4,
	display_horizontal_sync_start = 16,
	display_horizontal_sync_length = 32,
	display_vertical_blanking = 14,
	display_vertical_sync_start = 6,
	display_vertical_sync_length = 2,
	display_slot_width = 24,
	display_slot_height = 4,
	display_character_width = 16,
	display_character_width_padding = 3,
	display_character_height = 32,
	display_character_height_padding = 0,
	display_character_slot_padding = 16,
};

typedef struct
{
	unsigned int	unicode;
	unsigned int	internal;
} unicode_map_t;

static bool display_inited = false;
static unsigned int display_current_x, display_current_y, display_current_slot_offset;
static int display_current_slot;

roflash static const unicode_map_t unicode_map[] =
{
	{	0x263a, 0x01	},	//	☺ 
	{	0x263b, 0x02	},	//	☻ 
	{	0x2665, 0x03	},	//	♥ 
	{	0x25c6, 0x04	},	//	◆ 
	{	0x2663, 0x05	},	//	♣ 
	{	0x2660, 0x06	},	//	♠ 
	{	0x25cf, 0x07	},	//	● 
	{	0x25c8, 0x08	},	//	◈
	{	0x25ef, 0x09	},	//	◯ 
	{	0x25d9, 0x0a	},	//	◙ 
	{	0x2642, 0x0b	},	//	♂ 
	{	0x2640, 0x0c	},	//	♀ 
	{	0x266a,	0x0d	},	//	♪ 
	{	0x266b,	0x0e	},	//	♫ 
	{	0x263c,	0x0f	},	//	☼ 
	{	0x25b6,	0x10	},	//	▶ 
	{	0x25c0,	0x11	},	//	◀ 
	{	0x2195,	0x12	},	//	↕ 
	{	0x203c,	0x13	},	//	‼ 
	{	0x00b6,	0x14	},	//	¶ 
	{	0x00a7,	0x15	},	//	§ 
	{	0x2580,	0x16	},	//	▀ 
	{	0x21a8,	0x17	},	//	↨  
	{	0x2191,	0x18	},	//	↑ 
	{	0x2193,	0x19	},	//	↓ 
	{	0x2192,	0x1a	},	//	→ 
	{	0x2190,	0x1b	},	//	←
	{	0x250c,	0x1c	},	//	┌ 
	{	0x2194,	0x1d	},	//	↔  
	{	0x25b2,	0x1e	},	//	▲ 
	{	0x25bc,	0x1f	},	//	▼ 
	{	0x2588,	0x7f	},	//	█ 
	{	0x20ac,	0x80	},	//	€ 
	{	0x201a,	0x82	},	//	‚ 
	{	0x0192,	0x83	},	//	ƒ
	{	0x201e,	0x84	},	//	„
	{	0x2026,	0x85	},	//	…
	{	0x2020,	0x86	},	//	† 
	{	0x2021,	0x87	},	//	‡ 
	{	0x02c6,	0x88	},	//	ˆ 
	{	0x2030,	0x89	},	//	‰ 
	{	0x0160,	0x8a	},	//	Š
	{	0x2039,	0x8b	},	//	‹ 
	{	0x0152,	0x8c	},	//	Œ 
	{	0x017d,	0x8e	},	//	Ž
	{	0x2018,	0x91	},	//	‘ 
	{	0x2019,	0x92	},	//	’
	{	0x201c,	0x93	},	//	“ 
	{	0x201d,	0x94	},	//	” 
	{	0x2022,	0x95	},	//	•
	{	0x2013,	0x96	},	//	–
	{	0x2014,	0x97	},	//	—
	{	0x02dc,	0x98	},	//	˜
	{	0x2122,	0x99	},	//	™ 
	{	0x0161,	0x9a	},	//	š 
	{	0x203a,	0x9b	},	//	›
	{	0x0153,	0x9c	},	//	œ 
	{	0x017e,	0x9e	},	//	ž
	{	0x0178,	0x9f	},	//	Ÿ
	{	mapeof,	0x00	}, // EOF
};

enum
{
	i2c_addr_data = 0x06,
	i2c_addr_command = 0x07,
};

static bool display_write_command(uint8_t cmd)
{
	return(i2c_send1(i2c_addr_command, cmd) == i2c_error_ok);
}

static bool display_write_data(uint8_t data)
{
	return(i2c_send1(i2c_addr_data, data) == i2c_error_ok);
}

static bool display_write(uint8_t cmd, uint8_t data)
{
	if(!display_write_command(cmd))
		return(false);

	return(display_write_data(data));
}

static unsigned int display_text_current = 0;
static uint8_t display_text_buffer[32];

static unsigned int display_text_to_graphic_x(unsigned int text_x)
{
	return(text_x * (display_character_width + display_character_width_padding));
}

static unsigned int display_text_to_graphic_y(unsigned int slot_offset, unsigned int text_y)
{
	unsigned int graphic_y;
	unsigned int cell_height = display_character_height + display_character_height_padding;

	graphic_y = slot_offset * display_slot_height * cell_height;
	graphic_y += slot_offset * display_character_slot_padding;
	graphic_y += text_y * cell_height;

	return(graphic_y);
}

static void text_flush(void)
{
    display_write(reg_mwcr0, reg_mwcr0_mode_text | reg_mwcr0_cursor_invisible | reg_mwcr0_memory_write_direction_lrtd | reg_mwcr0_memory_write_autoincr_en | reg_mwcr0_memory_read_autoincr_en);
	display_write_command(reg_mrwc);

	i2c_send(i2c_addr_data, display_text_current, display_text_buffer);

	display_text_current = 0;
}

static void text_send(unsigned int text)
{
	if((display_text_current + 1) >= sizeof(display_text_buffer))
		text_flush();

	display_text_buffer[display_text_current++] = (uint8_t)text;
}

static void text_goto(unsigned int slot_offset, unsigned int textx, unsigned int texty)
{
	unsigned int x, y;

	x = display_text_to_graphic_x(textx) + 4;
	y = display_text_to_graphic_y(slot_offset, texty);

	text_flush();

	display_write(reg_curxl, (x >> 0) & 0xff);
	display_write(reg_curxh, (x >> 8) & 0x03);
	display_write(reg_curyl, (y >> 0) & 0xff);
	display_write(reg_curyh, (y >> 8) & 0x03);
}

static bool display_fgcolour(unsigned int r, unsigned int g, unsigned int b)
{
	if(!display_write(reg_fgcr0, r))
		return(false);

	if(!display_write(reg_fgcr1, g))
		return(false);

	if(!display_write(reg_fgcr2, b))
		return(false);

	return(true);
}

static bool display_bgcolour(unsigned int r, unsigned int g, unsigned int b)
{
	if(!display_write(reg_bgcr0, r))
		return(false);

	if(!display_write(reg_bgcr1, g))
		return(false);

	if(!display_write(reg_bgcr2, b))
		return(false);

	return(true);
}

static bool display_trcolour(unsigned int r, unsigned int g, unsigned int b)
{
	if(!display_write(reg_bgtr0, r))
		return(false);

	if(!display_write(reg_bgtr1, g))
		return(false);

	if(!display_write(reg_bgtr2, b))
		return(false);

	return(true);
}

static bool display_set_active_window(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
{
	x0 = umin(x0, display_width - 1);
	x1 = umin(x1, display_width - 1);
	y0 = umin(y0, display_height - 1);
	y1 = umin(y1, display_height - 1);

	if(!display_write(reg_hsaw0, (x0 >> 0) & 0xff))
		return(false);

	if(!display_write(reg_hsaw1, (x0 >> 8) & 0x03))
		return(false);

	if(!display_write(reg_vsaw0, (y0 >> 0) & 0xff))
		return(false);

	if(!display_write(reg_vsaw1, (y0 >> 8) & 0x01))
		return(false);

	if(!display_write(reg_heaw0, (x1 >> 0) & 0xff))
		return(false);

	if(!display_write(reg_heaw1, (x1 >> 8) & 0x03))
		return(false);

	if(!display_write(reg_veaw0, (y1 >> 0) & 0xff))
		return(false);

	if(!display_write(reg_veaw1, (y1 >> 8) & 0x01))
		return(false);

	return(true);
}

static bool display_set_active_layer(unsigned int layer)
{
	return(display_write(reg_mwcr1, reg_mwcr1_graphic_cursor_disable | reg_mwcr1_write_destination_layer | (layer & 0x01)));
}

static bool display_clear_area(unsigned int layer, unsigned int r, unsigned int g, unsigned int b)
{
	if(!display_set_active_layer(layer))
		return(false);

	if(!display_bgcolour(r, g, b))
		return(false);

	if(!display_write(reg_mclr, reg_mclr_memory_clear_start | reg_mclr_memory_area_active_window))
		return(false);

	msleep(8);

	if(!display_set_active_layer(0))
		return(false);

	return(true);
}

static bool display_fill_box(unsigned int layer, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int r, unsigned int g, unsigned int b)
{
	if(!display_set_active_window(x0, y0, x1, y1))
		return(false);

	if(!display_clear_area(layer, r, g, b))
		return(false);

	if(!display_set_active_window(0, 0, display_width, display_height))
		return(false);

	return(true);
}

bool display_eastrising_init(void)
{
#if 0
	// init PLL

	if(!display_write_command(reg_pll_c1))
		return(false);

	display_write_data(0x0a);	//	"PLL input parameter" (1-31)

	msleep(1);

	if(!display_write_command(reg_pll_c2))
		return(false);

	display_write_data(0x02);	//	divide by 4

	msleep(1);
#endif

	if(!display_write(reg_sysr, reg_sysr_color_depth_16 | reg_sysr_if_8bit))
		return(false);

	if(!display_write(reg_pcsr, reg_pcsr_sample_falling_edge | reg_pcsr_clock_period_system_by_4))
		return(false);

	// horizontal

	if(!display_write(reg_hdwr, (display_width / 8) - 1))
		return(false);

	if(!display_write(reg_hndftr, reg_hndftr_de_polarity_active_high | (display_horizontal_blanking_fine / 2)))
		return(false);

	if(!display_write(reg_hndr, (display_horizontal_blanking / 8) - 1))
		return(false);

	if(!display_write(reg_hstr, (display_horizontal_sync_start / 8) - 1))
		return(false);

	if(!display_write(reg_hpwr, reg_hpwr_hsync_polarity_active_low | ((display_horizontal_sync_length / 8) - 1)))
		return(false);

	// vertical

	if(!display_write(reg_vdhr0, ((display_height >> 0) & 0xff) + 1))
		return(false);

	if(!display_write(reg_vdhr1, ((display_height >> 8) & 0x01) + 0))
		return(false);

	if(!display_write(reg_vndr0, ((display_vertical_blanking >> 0) & 0xff) + 1))
		return(false);

	if(!display_write(reg_vndr1, ((display_vertical_blanking >> 8) & 0x01) + 0))
		return(false);

	if(!display_write(reg_vstr0, ((display_vertical_sync_start >> 0) & 0xff) + 1))
		return(false);

	if(!display_write(reg_vstr1, ((display_vertical_sync_start >> 8) & 0x01) + 0))
		return(false);

	if(!display_write(reg_vpwr, reg_vpwr_vsync_polarity_active_low | (display_vertical_sync_length - 1)))
		return(false);

	// PWM

	if(!display_write(reg_p1cr, reg_p1cr_pwm1_enable | reg_p1cr_function_pwm1 | reg_p1cr_clock_ratio_128))
		return(false);

	if(!display_write(reg_pwrr, reg_pwrr_display_enable | reg_pwrr_display_sleep_mode_disable | reg_pwrr_display_reset_complete))
		return(false);

	// MISC

	if(!display_write(reg_fncr0, reg_fncr0_font_cgrom | reg_fncr0_font_internal | reg_fncr0_encoding_8859_1))
		return(false);

	if(!display_write(reg_fncr1, reg_fncr1_font_align_disable | reg_fncr1_font_opaque | reg_fncr1_font_straight | reg_fncr1_font_enlarge_hor_x2 | reg_fncr1_font_enlarge_ver_x2))
		return(false);

	if(!display_write(reg_fncr2, reg_fncr2_font_size_16x16 | display_character_width_padding))
		return(false);

	if(!display_write(reg_dpcr, reg_dpcr_two_layer | reg_dpcr_hor_scan_ltor | reg_dpcr_vert_scan_ltor))
		return(false);

	if(!display_write(reg_ltpr0, reg_ltpr0_scroll_both | reg_ltpr0_floatwin_transparency_dis | reg_ltpr0_visible_layer_transparent))
		return(false);

	if(!display_write(reg_ltpr1, reg_ltpr1_transparency_layer_2_8_8 | reg_ltpr1_transparency_layer_1_8_8))
		return(false);

	if(!display_trcolour(0x00, 0x00, 0x00))
		return(false);

	if(!display_fill_box(0, 0, 0, display_width, display_height, 0x00, 0x00, 0x00))
		return(false);

	if(!display_fill_box(1, 0, 0, display_width, display_height, 0x00, 0x00, 0x00))
		return(false);

	if(!display_eastrising_bright(1))
		return(false);

	display_inited = true;

	return(true);
}

void display_eastrising_begin(int slot, unsigned int slot_offset)
{
	unsigned int r, g, b;
	unsigned int r1, g1, b1;
	unsigned int r2, g2, b2;

	if(!display_inited)
		log("! display eastrising not inited\n");

	display_current_x = display_current_y = 0;

	display_current_slot = slot;
	display_current_slot_offset = slot_offset;

	if(display_current_slot < 0)
	{
		r1 = g1 = b1 = 0x00;
		r2 = g2 = b2 = 0x00;
	}
	else
	{
		r = (display_current_slot & (1 << 0)) >> 0;
		g = (display_current_slot & (1 << 1)) >> 1;
		b = (display_current_slot & (1 << 2)) >> 2;

		r1 = r * 0x90;
		g1 = g * 0x90;
		b1 = b * 0x90;

		if((r1 + b1 + g1) == 0)
		{
			r1 = 0x88;
			g1 = 0x90;
			b1 = 0x88;
		}

		r2 = r * 0xff;
		g2 = g * 0xff;
		b2 = b * 0xff;

		if((r2 + b2 + g2) == 0)
		{
			r2 = 0xb0;
			g2 = 0xa0;
			b2 = 0xb0;
		}
	}

	display_fill_box(1, 0, display_text_to_graphic_y(slot_offset, 0) + 0, display_width, display_text_to_graphic_y(slot_offset, 1) + 2, r1, g1, b1);
	display_fill_box(1, 0, display_text_to_graphic_y(slot_offset, 1) + 2, display_width, display_text_to_graphic_y(slot_offset, 4) + 0, r2, g2, b2);

	if(g1 > 0x88)
		display_fgcolour(0x01, 0x01, 0x01);
	else
		display_fgcolour(0xff, 0xff, 0xff);

	display_bgcolour(0x00, 0x00, 0x00);

	text_goto(display_current_slot_offset, 0, 0);
}

void display_eastrising_output(unsigned int unicode)
{
	const unicode_map_t *unicode_map_ptr;

	if(unicode == '\n')
	{
		if(display_current_y < display_slot_height)
		{
			while(display_current_x++ < (display_slot_width + 1))
				text_send(' ');

			if(display_current_y < (display_slot_height - 1))
				text_goto(display_current_slot_offset, 0, display_current_y + 1);
		}

		display_current_x = 0;
		display_current_y++;

		return;
	}

	if((display_current_y < display_slot_height) && (display_current_x < (display_slot_width + 1)))
	{
		for(unicode_map_ptr = unicode_map; unicode_map_ptr->unicode != mapeof; unicode_map_ptr++)
			if(unicode_map_ptr->unicode == unicode)
			{
				unicode = unicode_map_ptr->internal;
				text_send(unicode);
				goto end;
			}

		if(((unicode >= ' ') && (unicode <= '}')) || ((unicode >= 0xa1) && (unicode <= 0xff)))
			text_send(unicode);
		else
			text_send(' ');
	}

end:
	display_current_x++;
}

void display_eastrising_end(void)
{
	if(display_current_x >= display_slot_width)
	{
		display_current_x = 0;
		display_current_y++;
	}

	for(; display_current_y < display_slot_height; display_current_y++, display_current_x = 0)
	{
		text_goto(display_current_slot_offset, display_current_x, display_current_y);

		while(display_current_x++ < (display_slot_width + 1))
			text_send(' ');
	}

	text_flush();
}

bool display_eastrising_bright(int brightness)
{
	roflash static const unsigned int bright_level[5] = { 0, 5, 20, 110, 255 };
	roflash static const unsigned int bright_power[5] = { reg_pwrr_display_disable, reg_pwrr_display_enable, reg_pwrr_display_enable, reg_pwrr_display_enable, reg_pwrr_display_enable };

	if(brightness > 4)
		return(false);

	if(!display_write(reg_p1dcr, bright_level[brightness]))
		return(false);

	if(!display_write(reg_pwrr, bright_power[brightness] | reg_pwrr_display_sleep_mode_disable | reg_pwrr_display_reset_complete))
		return(false);

	return(true);
}
