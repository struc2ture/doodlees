#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <freetype/freetype.h>

#define globvar static

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

    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size(ft_face, 50 * 64, 0, 100, 0);
    assert(!error);

    double angle = ( 25.0 / 360 ) * 3.14159 * 2; // use 25 degrees
    FT_GlyphSlot slot = ft_face->glyph;

    FT_Matrix matrix;
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

    int target_height = height;
    FT_Vector pen;
    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (300,200) relative to the upper left corner  */
    pen.x = 300 * 64;
    pen.y = ( target_height - 200 ) * 64;

    const char *text = "Andrey :)";
    int char_count = strlen(text);

    for (int i = 0; i < char_count; i++)
    {
        /* set transformation */
        FT_Set_Transform(ft_face, &matrix, &pen);

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(ft_face, text[i], FT_LOAD_RENDER);
        assert(!error);

        ppm_draw_ft_bitmap(
            &slot->bitmap,
            slot->bitmap_left,
            target_height - slot->bitmap_top);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }
    #endif

    ppm_write("out/out.ppm");

    FT_Done_Face    (ft_face);
    FT_Done_FreeType(ft_library);

    return 0;
}
