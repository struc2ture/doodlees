#include "font_loader.h"

#include "common/types.h"
#include "common/util.h"

#include <freetype/freetype.h>

static inline int truncate_to_int(float v)
{
    return (int)v;
}

static inline float convert_26dot6_float(FT_Pos x)
{
    return (float)(x / 64.0f);
}

void draw_ft_bitmap(FT_Bitmap *bitmap, int x_min, int y_min, unsigned char *pixels, int width, int height)
{
    int x_max = x_min + bitmap->width;
    int y_max = y_min + bitmap->rows;

    for (int x = x_min, p = 0; x < x_max; x++, p++)
    {
        for (int y = y_min, q = 0; y < y_max; y++, q++)
        {
            if (x < 0 || y < 0 || x >= width || y >= height)
            {
                continue;
            }
            pixels[x + y * width] = bitmap->buffer[q * bitmap->width + p];
        }
    }
}

FontAtlas font_loader_create_atlas(const char *path, int width, int height, float size, float dpi_scale)
{
    FT_Library ft_library;
    FT_Error error;
    FT_Face ft_face;

    error = FT_Init_FreeType(&ft_library);
    assert(!error);

    error = FT_New_Face(ft_library, path, 0, &ft_face);
    assert(!error);

    assert(FT_IS_SCALABLE(ft_face));

    error = FT_Set_Char_Size(ft_face, size * 64.0f, 0, 72.0f * dpi_scale, 0);
    assert(!error);

    float origin_x = 0.0f;
    float origin_y = 0.0f;

    float pad = 4.0f;

    float pen_x = origin_x + pad;
    float pen_y = origin_y + pad;

    int starting_ch = 32;
    int last_ch = 127;

    GlyphMetric *glyph_metrics = xcalloc(last_ch * sizeof(glyph_metrics[0]));

    float row_max_height = 0;

    unsigned char *pixels = xcalloc(width * height);

    for (int ch = starting_ch; ch < last_ch; ch++)
    {
        error = FT_Load_Char(ft_face, (unsigned char)ch, FT_LOAD_RENDER);
        assert(!error);

        bool will_fit = pen_x + (float)ft_face->glyph->bitmap.width + pad <= width;

        if (!will_fit)
        {
            pen_x = pad;
            pen_y += row_max_height + pad;
            row_max_height = 0;
        }

        draw_ft_bitmap(
            &ft_face->glyph->bitmap,
            truncate_to_int(pen_x),
            truncate_to_int(pen_y),
            pixels,
            width,
            height
        );

        GlyphMetric g = {};
        g.advance_x = convert_26dot6_float(ft_face->glyph->advance.x);
        g.offset_x = (float)ft_face->glyph->bitmap_left;
        g.offset_y = (float)ft_face->glyph->bitmap_top;
        g.width = (float)ft_face->glyph->bitmap.width;
        g.height = (float)ft_face->glyph->bitmap.rows;
        g.u0 = pen_x / (float)width;
        g.v0 = pen_y / (float)height;
        g.u1 = g.u0 + g.width / (float)width;
        g.v1 = g.v0 + g.height / (float)height;

        glyph_metrics[ch] = g;

        if (g.height + pad > row_max_height)
        {
            row_max_height = g.height + pad;
        }

        pen_x += g.width + pad;
    }

    FontAtlas result =
    {
        .pixels = pixels,
        .width = width,
        .height = height,
        .ascender = ft_face->size->metrics.ascender / 64.0f,
        .dpi_scale = dpi_scale,
        .glyph_metrics = glyph_metrics
    };

    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);

    return result;
}

GlyphQuad font_loader_get_glyph_quad(const FontAtlas *atlas, unsigned char ch, float x, float y)
{
    GlyphMetric *gm = atlas->glyph_metrics +ch;

    float screen_min_x = x + gm->offset_x / atlas->dpi_scale;
    float screen_min_y = y - gm->offset_y / atlas->dpi_scale;
    float screen_max_x = screen_min_x + gm->width / atlas->dpi_scale;
    float screen_max_y = screen_min_y + gm->height / atlas->dpi_scale;

    GlyphQuad q =
    {
        .screen_min = V2(screen_min_x, screen_min_y),
        .screen_max = V2(screen_max_x, screen_max_y),
        .tex_min = V2(gm->u0, gm->v0),
        .tex_max = V2(gm->u1, gm->v1),
    };

    return q;
}

f32 font_loader_get_advance_x(const FontAtlas *atlas, unsigned char ch)
{
    GlyphMetric *gm = atlas->glyph_metrics +ch;
    return gm->advance_x / atlas->dpi_scale;
}

f32 font_loader_get_ascender(const FontAtlas *atlas)
{
    return atlas->ascender / atlas->dpi_scale;
}
