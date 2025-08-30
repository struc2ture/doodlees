#pragma once

#include <freetype/freetype.h>

typedef struct PpmPixel
{
    unsigned char d[3];
} PpmPixel;

void ppm_write_px(const char *path, void *pixels, int width, int height);
void ppm_write_px4(const char *path, void *px4, int width, int height);
void ppm_write_px1(const char *path, void *px1, int width, int height);
void ppm_init(int width, int height);
void ppm_write(const char *path);
void ppm_draw_px(int x, int y, PpmPixel p);
PpmPixel ppm_get_px(int x, int y);
PpmPixel pixel_blend(PpmPixel dst, PpmPixel src, unsigned char src_coverage);
void ppm_draw_ft_bitmap(FT_Bitmap *bitmap, int x_min, int y_min);
