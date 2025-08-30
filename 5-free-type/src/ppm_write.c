#include <assert.h>
#include <stdio.h>

#include <freetype/freetype.h>

#include "common/types.h"
#include "common/util.h"

#include "ppm_write.h"

typedef struct PpmPixel
{
    unsigned char d[3];
} PpmPixel;

globvar PpmPixel *ppm_pixels;
globvar int ppm_width;
globvar int ppm_height;

void ppm_write_px(const char *path, void *pixels, int width, int height)
{
    FILE *f = fopen(path, "wb");
    assert(f);
    // Write PPM header
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    fwrite(pixels, width * height * 3, 1, f);
    fclose(f);
}

void ppm_write_px4(const char *path, void *px4, int width, int height)
{
    PpmPixel *pixels = xmalloc(width * height * sizeof(pixels[0]));
    unsigned char *src_px = px4;
    const int src_bpp = 4;
    const int src_stride = width * src_bpp;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            pixels[col+row*width].d[0] = src_px[col*src_bpp+row*src_stride+0];
            pixels[col+row*width].d[1] = src_px[col*src_bpp+row*src_stride+1];
            pixels[col+row*width].d[2] = src_px[col*src_bpp+row*src_stride+2];
        }
    }
    ppm_write_px(path, pixels, width, height);
    free(pixels);
}

void ppm_write_px1(const char *path, void *px1, int width, int height)
{
    PpmPixel *pixels = xmalloc(width * height * sizeof(pixels[0]));
    unsigned char *src_px = px1;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            pixels[col+row*width].d[0] = src_px[col+row*width];
            pixels[col+row*width].d[1] = src_px[col+row*width];
            pixels[col+row*width].d[2] = src_px[col+row*width];
        }
    }
    ppm_write_px(path, pixels, width, height);
    free(pixels);
}
