#include <stdio.h>

#include <freetype/freetype.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "common/types.h"
#include "common/util.h"

#include "ppm_write.h"

#include "ft_gl_renderer.c"

static inline int truncate_to_int(float v)
{
    return (int)v;
}

static inline float convert_26dot6_float(FT_Pos x)
{
    return (float)(x / 64.0f);
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

unsigned char *atlas_pixels;
int atlas_width;
int atlas_height;

globvar float font_ascender;

globvar float dpi_scale;

void draw_ft_bitmap(FT_Bitmap *bitmap, int x_min, int y_min)
{
    int x_max = x_min + bitmap->width;
    int y_max = y_min + bitmap->rows;

    for (int x = x_min, p = 0; x < x_max; x++, p++)
    {
        for (int y = y_min, q = 0; y < y_max; y++, q++)
        {
            if (x < 0 || y < 0 || x >= atlas_width || y >= atlas_height)
            {
                continue;
            }
            atlas_pixels[x + y * atlas_width] = bitmap->buffer[q * bitmap->width + p];
        }
    }
}

void render_glyph(unsigned char ch, float *pen_x, float *pen_y)
{
    GlyphMetric *gm = glyph_metrics + ch;

    float x = *pen_x;
    float y = *pen_y + font_ascender / dpi_scale;

    float screen_min_x = x + gm->offset_x / dpi_scale;
    float screen_min_y = y - gm->offset_y / dpi_scale;
    float screen_max_x = screen_min_x + gm->width / dpi_scale;
    float screen_max_y = screen_min_y + gm->height / dpi_scale;

    renderer_draw(
        V2(screen_min_x, screen_max_y),
        V2(screen_max_x, screen_max_y),
        V2(screen_max_x, screen_min_y),
        V2(screen_min_x, screen_min_y),
        // TexCoords in reverse order to flip the quad
        V2(gm->u0, gm->v1),
        V2(gm->u1, gm->v1),
        V2(gm->u1, gm->v0),
        V2(gm->u0, gm->v0),
        V4(1.0f, 1.0f, 1.0f, 1.0f)
    );

    *pen_x += gm->advance_x / dpi_scale;
}

void render_string(const char *str, float *pen_x, float *pen_y)
{
    int len = strlen(str);
    float starting_x = *pen_x;
    for (int i = 0; i < len; i++)
    {
        if (str[i] != '\n')
        {
            render_glyph(str[i], pen_x, pen_y);
        }
        else
        {
            *pen_y += font_ascender / dpi_scale;
            *pen_x = starting_x;
        }
    }
}

// MAIN

globvar GLFWwindow *window;

void init()
{
    atlas_width = 512;
    atlas_height = 512;
    atlas_pixels = xcalloc(atlas_width * atlas_height);

    dpi_scale = 2.0f;

    renderer_init();

    FT_Error error;
    error = FT_Init_FreeType(&ft_library);
    assert(!error);

    error = FT_New_Face(ft_library, "res/DMMono-Regular.ttf", 0, &ft_face);
    assert(!error);

    int scalable = FT_IS_SCALABLE(ft_face);
    int kerning = FT_HAS_KERNING(ft_face);

    float dpi_scale = 2.0f;
    error = FT_Set_Char_Size(ft_face, 32.0f * 64.0f, 0, 72.0f * dpi_scale, 0);
    assert(!error);

    font_ascender = ft_face->size->metrics.ascender / 64.0f;

    int target_height = atlas_height;

    float origin_x = 0.0f;
    float origin_y = 0.0f;

    float pad = 4.0f;

    float pen_x = origin_x + pad;
    float pen_y = origin_y + pad;

    int starting_ch = 32;
    int last_ch = 127;

    glyph_metrics = calloc(1, last_ch * sizeof(glyph_metrics[0]));
    int glyph_i = 0;

    float row_max_height = 0;

    for (int ch = starting_ch; ch < last_ch; ch++)
    {
        error = FT_Load_Char(ft_face, (unsigned char)ch, FT_LOAD_RENDER);
        assert(!error);

        bool will_fit = pen_x + (float)ft_face->glyph->bitmap.width + pad <= atlas_width;

        if (!will_fit)
        {
            pen_x = pad;
            pen_y += row_max_height + pad;
            row_max_height = 0;
        }

        draw_ft_bitmap(
            &ft_face->glyph->bitmap,
            truncate_to_int(pen_x),
            truncate_to_int(pen_y)
        );

        GlyphMetric g = {};
        g.advance_x = convert_26dot6_float(ft_face->glyph->advance.x);
        g.offset_x = (float)ft_face->glyph->bitmap_left;
        g.offset_y = (float)ft_face->glyph->bitmap_top;
        g.width = (float)ft_face->glyph->bitmap.width;
        g.height = (float)ft_face->glyph->bitmap.rows;
        g.u0 = pen_x / (float)atlas_width;
        g.v0 = pen_y / (float)atlas_height;
        g.u1 = g.u0 + g.width / (float)atlas_width;
        g.v1 = g.v0 + g.height / (float)atlas_height;

        glyph_metrics[ch] = g;

        if (g.height + pad > row_max_height)
        {
            row_max_height = g.height + pad;
        }

        pen_x += g.width + pad;
    }

    ppm_write_px1("out/atlas2.ppm", atlas_pixels, atlas_width, atlas_height);

    renderer_init_tex_from_px(atlas_pixels, atlas_width, atlas_height);
}

void frame()
{
    const int starting_ch = 32;
    const int last_ch = 127;

    float pen_x = 5.0f;
    float pen_y = 5.0f;

    const char *text = "LIA, I love you :)\n    - Andrey";
    render_string(text, &pen_x, &pen_y);

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    renderer_render(V2(width, height));
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    window = glfwCreateWindow(1000, 900, "game", NULL, NULL);
    glfwMakeContextCurrent(window);

    trace("OpenGL Vendor: %s", glGetString(GL_VENDOR));
    trace("OpenGL Renderer: %s", glGetString(GL_RENDERER));
    trace("OpenGL Version: %s", glGetString(GL_VERSION));

    // glfwSetCharCallback(window, on_char);
    // glfwSetKeyCallback(window, on_key);
    // glfwSetCursorPosCallback(window, on_mouse_cursor);
    // glfwSetMouseButtonCallback(window, on_mouse_button);
    // glfwSetScrollCallback(window, on_scroll);
    // glfwSetFramebufferSizeCallback(window, on_framebuffer_size);
    // glfwSetWindowSizeCallback(window, on_window_size);

    glfwSwapInterval(1);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        glViewport(0, 0, fb_w, fb_h);

        glClearColor(0.18f, 0.26f, 0.26f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        frame();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
