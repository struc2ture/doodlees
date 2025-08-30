#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <freetype/freetype.h>

#define globvar static
#define bp() __builtin_debugtrap()

static inline int truncate_to_int(float v)
{
    return (int)v;
}

static inline float convert_26dot6_float(FT_Pos x)
{
    return (float)(x / 64.0f);
}

// PPM

typedef struct Pixel
{
    unsigned char d[3];
} Pixel;

Pixel pixel_blend(Pixel dst, Pixel src, unsigned char src_coverage)
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
    return (Pixel){{
        (unsigned char)(dst_r * 255.0f),
        (unsigned char)(dst_g * 255.0f),
        (unsigned char)(dst_b * 255.0f),
    }};
}

globvar Pixel *ppm_pixels;
globvar int ppm_width;
globvar int ppm_height;

void ppm_init(int width, int height)
{
    ppm_pixels = malloc(width * height * sizeof(ppm_pixels[0]));
    assert(ppm_pixels);

    for (int i = 0; i < width * height; i++)
    {
        ppm_pixels[i] = (Pixel){255, 255, 255};
    }

    ppm_width = width;
    ppm_height = height;
}

void ppm_draw_px(int x, int y, Pixel p)
{
    ppm_pixels[ppm_width * y + x] = p;
}

Pixel ppm_get_px(int x, int y)
{
    return ppm_pixels[ppm_width * y + x];
}

void ppm_write(const char *path)
{
    FILE *f = fopen(path, "wb");
    assert(f);
    // Write PPM header
    fprintf(f, "P6\n%d %d\n255\n", ppm_width, ppm_height);
    fwrite(ppm_pixels, ppm_width * ppm_height * sizeof(ppm_pixels[0]), 1, f);
    fclose(f);
}

// Free type
globvar FT_Library ft_library;
globvar FT_Face ft_face;

typedef struct GlyphMetric
{
    float advance_x;
    float offset_x;
    float offset_y;
    float width;
    float height;

    float u0;
    float v0;
    float u1;
    float v1;
} GlyphMetric;

GlyphMetric *glyph_metrics;

void ppm_draw_ft_bitmap(FT_Bitmap *bitmap, int x_min, int y_min)
{
    int x_max = x_min + bitmap->width;
    int y_max = y_min + bitmap->rows;

    Pixel black = {0};
    for (int x = x_min, p = 0; x < x_max; x++, p++)
    {
        for (int y = y_min, q = 0; y < y_max; y++, q++)
        {
            if (x < 0 || y < 0 || x >= ppm_width || y >= ppm_height)
            {
                continue;
            }

            unsigned char coverage = bitmap->buffer[q * bitmap->width + p];
            Pixel p = pixel_blend(ppm_get_px(x, y), black, coverage);
            ppm_draw_px(x, y, p);
        }
    }
}

int main()
{
    const int width = 640;
    const int height = 480;
    ppm_init(width, height);

    #if 1
    FT_Error error;
    error = FT_Init_FreeType(&ft_library);
    assert(!error);

    error = FT_New_Face(ft_library, "res/DMMono-Regular.ttf", 0, &ft_face);
    assert(!error);

    int scalable = FT_IS_SCALABLE(ft_face);
    int kerning = FT_HAS_KERNING(ft_face);

    float dpi_scale = 2.0f;
    error = FT_Set_Char_Size(ft_face, 14 * 64, 0, 72 * dpi_scale, 0);
    assert(!error);

    int target_height = height;

    float origin_x = 0.0f;
    float origin_y = 0.0f;

    float pad = 4.0f;

    float pen_x = origin_x + pad;
    float pen_y = origin_y + pad;

    int starting_ch = 32;
    int last_ch = 127;

    int total_count = last_ch - starting_ch;
    glyph_metrics = calloc(1, total_count * sizeof(glyph_metrics[0]));
    int glyph_i = 0;

    float row_max_height = 0;

    for (int ch = starting_ch; ch < last_ch; ch++)
    {
        error = FT_Load_Char(ft_face, (unsigned char)ch, FT_LOAD_RENDER);
        assert(!error);

        ppm_draw_ft_bitmap(
            &ft_face->glyph->bitmap,
            truncate_to_int(pen_x + pad),
            truncate_to_int(pen_y + pad)
        );

        GlyphMetric g = {};
        g.advance_x = convert_26dot6_float(ft_face->glyph->advance.x);
        g.offset_x = (float)ft_face->glyph->bitmap_left;
        g.offset_y = (float)ft_face->glyph->bitmap_top;
        g.width = (float)ft_face->glyph->bitmap.width;
        g.height = (float)ft_face->glyph->bitmap.rows;
        g.u0 = pen_x + pad;
        g.v0 = pen_y + pad;
        g.u1 = pen_x + pad + ft_face->glyph->bitmap.width;
        g.v1 = pen_y + pad + ft_face->glyph->bitmap.rows;

        glyph_metrics[glyph_i++] = g;

        if (ft_face->glyph->bitmap.rows + pad > row_max_height)
        {
            row_max_height = g.height + pad;
        }

        if (pen_x + pad + g.height > ppm_width)
        {
            pen_x = pad;
            pen_y += row_max_height + pad;
            row_max_height = 0;
        }
        else
        {
            pen_x += g.height + pad;
        }
    }
    #endif

    ppm_write("out/out.ppm");

    bp();

    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);

    return 0;
}
