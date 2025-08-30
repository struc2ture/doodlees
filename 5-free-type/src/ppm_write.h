#pragma once

#include <freetype/freetype.h>

void ppm_write_px(const char *path, void *pixels, int width, int height);
void ppm_write_px4(const char *path, void *px4, int width, int height);
void ppm_write_px1(const char *path, void *px1, int width, int height);
