#pragma once

#include "common/types.h"

typedef struct GlyphMetric
{
    f32 advance_x;
    f32 offset_x;
    f32 offset_y;
    f32 width;
    f32 height;

    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;
} GlyphMetric;

typedef struct FontAtlas
{
    void *pixels;
    int width;
    int height;
    f32 ascender;
    f32 dpi_scale;
    GlyphMetric *glyph_metrics;
} FontAtlas;

typedef struct GlyphQuad
{
    v2 screen_min;
    v2 screen_max;
    v2 tex_min;
    v2 tex_max;
} GlyphQuad;

FontAtlas font_loader_create_atlas(const char *path, int width, int height, float size, float dpi_scale);
GlyphQuad font_loader_get_glyph_quad(const FontAtlas *atlas, unsigned char ch, float x, float y);
f32 font_loader_get_advance_x(const FontAtlas *atlas, unsigned char ch);
f32 font_loader_get_ascender(const FontAtlas *atlas);
