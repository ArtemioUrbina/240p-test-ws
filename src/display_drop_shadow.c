/**
 * Copyright (c) 2024 Adrian Siekierka
 *
 * 240p-test-ws is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * 240p-test-ws is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with 240p-test-ws. If not, see <https://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <wonderful.h>
#include <ws.h>

#include "../build/assets/color/pyramid.h"
#include "../build/assets/mono/pyramid.h"
#include "../build/assets/mono/star_top_left.h"

#include "iram.h"
#include "main.h"

void display_drop_shadow(void *userdata) {
	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));
	outportb(IO_SPR_BASE, SPR_BASE(sprites));
	outportb(IO_SPR_FIRST, 0);
	outportb(IO_SPR_COUNT, 16);

	// Configure background, tile data.
	if (ws_system_is_color()) {
		ws_system_mode_set(WS_MODE_COLOR_4BPP);
		memset(MEM_TILE_4BPP_BANK0(0), 0, sizeof(ws_tile_4bpp_t));
		tile_copy_2bpp_to_4bpp(MEM_TILE_4BPP_BANK0(384), gfx_mono_star_top_left, 4 * 16);
		ws_dma_copy_words(MEM_COLOR_PALETTE(0), gfx_color_pyramid_palette, gfx_color_pyramid_palette_size);
		ws_dma_copy_words(MEM_TILE_4BPP_BANK1(1), gfx_color_pyramid_tiles, gfx_color_pyramid_tiles_size);
		MEM_COLOR_PALETTE(12)[1] = 0x000;
		ws_screen_put_tiles(screen_1, gfx_color_pyramid_map, 0, 0, 28, 18);
		ws_screen_modify_tiles(screen_1, ~SCR_ENTRY_BANK_MASK, SCR_ENTRY_BANK(1), 0, 0, 28, 18);
	} else {
		ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
		memset(MEM_TILE(0), 0, sizeof(ws_tile_t));
		memcpy(MEM_TILE(384), gfx_mono_star_top_left, 4 * 16);
		memcpy(MEM_TILE(1), gfx_mono_pyramid_tiles, gfx_mono_pyramid_tiles_size);
		ws_portcpy(IO_SCR_PAL_0, gfx_mono_pyramid_palette, gfx_mono_pyramid_palette_size);
		outportw(IO_SCR_PAL_12, MONO_PAL_COLORS(7, 7, 7, 7));
		ws_screen_put_tiles(screen_1, gfx_mono_pyramid_map, 0, 0, 28, 18);
	}

	// Configure sprite.
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	sprites[ 0].attr = 384 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY;
	sprites[ 1].attr = 385 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY;
	sprites[ 2].attr = 385 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H;
	sprites[ 3].attr = 384 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H;
	sprites[ 4].attr = 386 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY;
	sprites[ 5].attr = 387 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY;
	sprites[ 6].attr = 387 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H;
	sprites[ 7].attr = 386 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H;
	sprites[ 8].attr = 386 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_V;
	sprites[ 9].attr = 387 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_V;
	sprites[10].attr = 387 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H | SPR_ATTR_FLIP_V;
	sprites[11].attr = 386 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H | SPR_ATTR_FLIP_V;
	sprites[12].attr = 384 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_V;
	sprites[13].attr = 385 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_V;
	sprites[14].attr = 385 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H | SPR_ATTR_FLIP_V;
	sprites[15].attr = 384 | SPR_ATTR_PALETTE(4) | SPR_ATTR_PRIORITY | SPR_ATTR_FLIP_H | SPR_ATTR_FLIP_V;

	uint8_t sx = (SCR_WIDTH_PX - 32) >> 1;
	uint8_t sy = (SCR_HEIGHT_PX - 32) >> 1;

	while (true) {
		vblank_wait();
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

		uint16_t keys_pressed = scan_keys();
		if (curr_keys & KEY_X1) sy--;
		if (curr_keys & KEY_X3) sy++;
		if (curr_keys & KEY_X4) sx--;
		if (curr_keys & KEY_X2) sx++;
		if (keys_pressed & (KEY_A | KEY_B | KEY_START)) break;

		sprites[ 0].x = sx +  0; sprites[ 0].y = sy +  0;
		sprites[ 1].x = sx +  8; sprites[ 1].y = sy +  0;
		sprites[ 2].x = sx + 16; sprites[ 2].y = sy +  0;
		sprites[ 3].x = sx + 24; sprites[ 3].y = sy +  0;
		sprites[ 4].x = sx +  0; sprites[ 4].y = sy +  8;
		sprites[ 5].x = sx +  8; sprites[ 5].y = sy +  8;
		sprites[ 6].x = sx + 16; sprites[ 6].y = sy +  8;
		sprites[ 7].x = sx + 24; sprites[ 7].y = sy +  8;
		sprites[ 8].x = sx +  0; sprites[ 8].y = sy + 16;
		sprites[ 9].x = sx +  8; sprites[ 9].y = sy + 16;
		sprites[10].x = sx + 16; sprites[10].y = sy + 16;
		sprites[11].x = sx + 24; sprites[11].y = sy + 16;
		sprites[12].x = sx +  0; sprites[12].y = sy + 24;
		sprites[13].x = sx +  8; sprites[13].y = sy + 24;
		sprites[14].x = sx + 16; sprites[14].y = sy + 24;
		sprites[15].x = sx + 24; sprites[15].y = sy + 24;

		vblank_wait();
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SPR_ENABLE);
	}
}

