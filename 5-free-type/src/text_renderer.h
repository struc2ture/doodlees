#pragma once

#include "common/types.h"

#include "font_loader.h"

void tr_init();
void tr_init_tex_from_px(void *pixels, int width, int height);
void tr_init_atlas(FontAtlas atlas);
void tr_draw(v2 a, v2 b, v2 c, v2 d, v2 t_a, v2 t_b, v2 t_c, v2 t_d, v4 color);
void tr_draw_glyph(unsigned char ch, f32 *pen_x, f32 *pen_y);
void tr_draw_string(const char *str, f32 *pen_x, f32 *pen_y);
v2 tr_get_string_dim(const char *str);
void tr_render(v2 window_size);
