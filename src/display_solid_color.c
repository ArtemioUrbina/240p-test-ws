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

#include <stdio.h>
#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <ws/display.h>
#include <ws/hardware.h>

#include "iram.h"
#include "main.h"
#include "submenu.h"

static const char __wf_rom red_format[] = "Red: %d";
static const char __wf_rom green_format[] = "Green: %d";
static const char __wf_rom blue_format[] = "Blue: %d";
static const char __wf_rom shade_format[] = "Shade: %d";

static void submenu_rgb_draw(int id, char* buf, void* userdata) {
    const char __wf_rom *format;
    int shift;
    if (id == 0) { format = red_format; shift = 8; }
    if (id == 1) { format = green_format; shift = 4; }
    if (id == 2) { format = blue_format; shift = 0; }
    sprintf(buf, format, (MEM_COLOR_PALETTE(0)[0] >> shift) & 0xF);
}

static void submenu_shade_draw(int id, char* buf, void* userdata) {
    sprintf(buf, shade_format, inportb(IO_LCD_SHADE_45) >> 4);
}

#define KEY_HANDLER(col, shift) \
    if (keys & KEY_X4) col = (col & ~(0xF << shift)) | ((col - (1 << shift)) & (0xF << shift)); \
    if (keys & KEY_X2) col = (col & ~(0xF << shift)) | ((col + (1 << shift)) & (0xF << shift));

static void submenu_rgb_key  (int id, uint16_t keys, void* userdata) {
    KEY_HANDLER(MEM_COLOR_PALETTE(0)[0], ((2 - id) * 4));
}

static void submenu_shade_key  (int id, uint16_t keys, void* userdata) {
    uint8_t shade = inportb(IO_LCD_SHADE_45);
    if (keys & KEY_X4) shade -= 0x10;
    if (keys & KEY_X2) shade += 0x10;
    outportb(IO_LCD_SHADE_45, shade);
}

static const submenu_entry_t __wf_rom submenu_rgb[] = {
    SUBMENU_ENTRY(submenu_rgb, 0),
    SUBMENU_ENTRY(submenu_rgb, 0),
    SUBMENU_ENTRY(submenu_rgb, 0),
    SUBMENU_ENTRY_END()
};

static const submenu_entry_t __wf_rom submenu_shade[] = {
    SUBMENU_ENTRY(submenu_shade, 0),
    SUBMENU_ENTRY_END()
};

void display_solid_color(void *userdata) {
	submenu_state_t submenu;
	submenu.x = 0;
	submenu.y = 0;
	submenu.tile_start = 1;
	submenu.width = 6;
    submenu.tick = NULL;

	outportw(IO_DISPLAY_CTRL, 0);
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));
	ws_screen_fill_tiles(screen_1, 0, 0, 0, 28, 18);
    memset(MEM_TILE(0), 0, 16);

	if (ws_system_is_color()) {
        // Use color 0
		ws_system_mode_set(WS_MODE_COLOR);
        MEM_COLOR_PALETTE(0)[0] = RGB(15, 15, 15);
        submenu.entries = submenu_rgb;
	} else {
        // Use shade LUT entry 5
		ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
        outportw(IO_SCR_PAL_0, MONO_PAL_COLORS(4, 4, 4, 4));
        submenu.entries = submenu_shade;
	}

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);
	submenu_init(&submenu);

	while (true) {
		uint16_t keys_pressed = submenu_loop(&submenu);
		if (keys_pressed & (KEY_A | KEY_B | KEY_START)) break;
	}
}
