#include <assert.h>
#include <stdio.h>

#include <freetype/freetype.h>

#include "common/types.h"
#include "common/util.h"

#include "ppm_write.h"

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

void ppm_init(int width, int height)
{
    ppm_pixels = malloc(width * height * sizeof(ppm_pixels[0]));
    assert(ppm_pixels);

    for (int i = 0; i < width * height; i++)
    {
        ppm_pixels[i] = (PpmPixel){255, 255, 255};
    }

    ppm_width = width;
    ppm_height = height;
}

void ppm_write(const char *path)
{
    ppm_write_px(path, ppm_pixels, ppm_width, ppm_height);
}

void ppm_draw_px(int x, int y, PpmPixel p)
{
    ppm_pixels[ppm_width * y + x] = p;
}

PpmPixel ppm_get_px(int x, int y)
{
    return ppm_pixels[ppm_width * y + x];
}

PpmPixel pixel_blend(PpmPixel dst, PpmPixel src, unsigned char src_coverage)
{
    float alpha = (float)src_coverage / 255.0f;
    float src_r, src_g, src_b;
    src_r = src.d[0] / 255.0f;
    src_g = src.d[1] / 255.0f;
    src_b = src.d[2] / 255.0f;
    float dst_r, dst_g, dst_b;
    dst_r = dst.d[0] / 255.0f;
    dst_g = dst.d[1] / 255.0f;
    dst_b = dst.d[2] / 255.0f;

    dst_r = src_r * alpha + dst_r * (1 - alpha);
    dst_g = src_g * alpha + dst_g * (1 - alpha);
    dst_b = src_b * alpha + dst_b * (1 - alpha);
    return (PpmPixel){{
        (unsigned char)(dst_r * 255.0f),
        (unsigned char)(dst_g * 255.0f),
        (unsigned char)(dst_b * 255.0f)
    }};
}

void ppm_draw_ft_bitmap(FT_Bitmap *bitmap, int x_min, int y_min)
{
    int x_max = x_min + bitmap->width;
    int y_max = y_min + bitmap->rows;

    PpmPixel black = {0};
    for (int x = x_min, p = 0; x < x_max; x++, p++)
    {
        for (int y = y_min, q = 0; y < y_max; y++, q++)
        {
            if (x < 0 || y < 0 || x >= ppm_width || y >= ppm_height)
            {
                continue;
            }

            unsigned char coverage = bitmap->buffer[q * bitmap->width + p];
            PpmPixel p = pixel_blend(ppm_get_px(x, y), black, coverage);
            ppm_draw_px(x, y, p);
        }
    }
}
