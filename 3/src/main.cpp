#include <cstdio>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "util.hpp"

#include "gl_tiles.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct Rect
{
    v2 min;
    v2 ext;

    v2 get_min()
    {
        return min;
    }
    v2 get_max()
    {
        return (v2){{{min.x + ext.x, min.y + ext.y}}};
    }
    v2 get_a()
    {
        return min;
    }
    v2 get_b()
    {
        return (v2){{{min.x, min.y + ext.y}}};
    }
    v2 get_c()
    {
        return (v2){{{min.x + ext.x, min.y + ext.y}}};
    }
    v2 get_d()
    {
        return (v2){{{min.x + ext.x, min.y}}};
    }
};

Rect get_rect_for_tilemap_pos(int row, int col)
{
    const int tilemap_rows = 16;
    const int tilemap_cols = 16;
    f32 q_w = 1.0f / tilemap_rows;
    f32 q_h = 1.0f / tilemap_cols;
    Rect result = {
        .min = (v2){{{q_w * col, 1.0f - q_h * row}}},
        .ext = (v2){{{q_w, -q_h}}}
    };
    return result;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(1000, 900, "game", NULL, NULL);
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

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    // ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(main_scale);
    // style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_demo_window = true;

    GLuint tiles_shader = GLTiles::gl_create_tiles_shader();
    GLTiles::Vert_Buf *vb = GLTiles::vb_make();

    GLTiles::Texture tex = GLTiles::gl_load_texture("res/tileset.png", GL_NEAREST, true);
    trace("tex: %d; ch: %d", tex.texture_id, tex.ch);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            trace("iconified");
            continue;
        }

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.86f, 0.18f, 0.26f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(tiles_shader);

        GLTiles::vb_clear(vb);

        {
            int index_base = GLTiles::vb_next_vert_index(vb);
            v2 a = (v2){{{-0.5f,  0.5f}}};
            v2 b = (v2){{{-0.5f, -0.5f}}};
            v2 c = (v2){{{ 0.5f, -0.5f}}};
            v2 d = (v2){{{ 0.5f,  0.5f}}};

            int glyph_row = 0;
            int glyph_col = 1;
            Rect q = get_rect_for_tilemap_pos(glyph_row, glyph_col);
            trace("glyph rect: %f, %f, %f, %f", q.min.x, q.min.y, q.ext.x, q.ext.y);

            v2 t_a = q.get_a();
            v2 t_b = q.get_b();
            v2 t_c = q.get_c();
            v2 t_d = q.get_d();

            v4 fg_color = (v4){{{0.0f, 0.0f, 1.0f, 1.0f}}};
            v4 bg_color = (v4){{{1.0f, 1.0f, 1.0f, 1.0f}}};

            glBindTexture(GL_TEXTURE_2D, tex.texture_id);

            GLTiles::vb_add_vert(vb, GLTiles::make_vert(a, t_a, fg_color, bg_color));
            GLTiles::vb_add_vert(vb, GLTiles::make_vert(b, t_b, fg_color, bg_color));
            GLTiles::vb_add_vert(vb, GLTiles::make_vert(c, t_c, fg_color, bg_color));
            GLTiles::vb_add_vert(vb, GLTiles::make_vert(d, t_d, fg_color, bg_color));

            GLTiles::vb_add_indices(vb, index_base, (int[]){0, 1, 3, 1, 2, 3}, 6);
        }
        {
            int index_base = GLTiles::vb_next_vert_index(vb);
            v2 a = (v2){{{-0.5f,  0.5f}}};
            v2 b = (v2){{{-0.5f, -0.5f}}};
            v2 c = (v2){{{ 0.5f, -0.5f}}};
            v2 d = (v2){{{ 0.5f,  0.5f}}};

            int glyph_row = 3;
            int glyph_col = 3;
            Rect q = get_rect_for_tilemap_pos(glyph_row, glyph_col);
            trace("glyph rect: %f, %f, %f, %f", q.min.x, q.min.y, q.ext.x, q.ext.y);

            v2 t_a = q.get_a();
            v2 t_b = q.get_b();
            v2 t_c = q.get_c();
            v2 t_d = q.get_d();

            v4 fg_color = (v4){{{0.0f, 1.0f, 0.0f, 0.5f}}};
            v4 bg_color = (v4){{{1.0f, 1.0f, 1.0f, 0.5f}}};

            glBindTexture(GL_TEXTURE_2D, tex.texture_id);

            // GLTiles::vb_add_vert(vb, GLTiles::make_vert(a, t_a, fg_color, bg_color));
            // GLTiles::vb_add_vert(vb, GLTiles::make_vert(b, t_b, fg_color, bg_color));
            // GLTiles::vb_add_vert(vb, GLTiles::make_vert(c, t_c, fg_color, bg_color));
            // GLTiles::vb_add_vert(vb, GLTiles::make_vert(d, t_d, fg_color, bg_color));

            // GLTiles::vb_add_indices(vb, index_base, (int[]){0, 1, 3, 1, 2, 3}, 6);
        }

        GLTiles::vb_draw_call(vb);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

