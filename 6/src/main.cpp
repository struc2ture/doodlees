#include <cstdio>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "util.hpp"

#include "gl_tiles.cpp"
#include "game.cpp"

void on_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        GameState *gs = get_game_state();
        if (action == GLFW_PRESS)
        {
            gs->mouse_left_clicked = true;
        }
        else
        {
            gs->mouse_left_clicked = false;
        }
    }
}

void on_mouse_cursor(GLFWwindow* window, double xpos, double ypos)
{
    GameState *gs = get_game_state();
    gs->mouse_pos.x = (f32)xpos;
    gs->mouse_pos.y = (f32)ypos;
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
    glfwSetCursorPosCallback(window, on_mouse_cursor);
    glfwSetMouseButtonCallback(window, on_mouse_button);
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

    GLTiles::Vert_Buf *vb = GLTiles::vb_make();
    game_init(vb);

    GLuint tiles_shader = GLTiles::gl_create_tiles_shader();

    GLTiles::Texture tex = GLTiles::gl_load_texture("res/tileset.png", GL_NEAREST, true);
    trace("tex: %d; ch: %d", tex.texture_id, tex.ch);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    f32 delta = 1/120.0f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            trace("iconified");
            continue;
        }

        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        glViewport(0, 0, fb_w, fb_h);

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.86f, 0.18f, 0.26f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(tiles_shader);
        glBindTexture(GL_TEXTURE_2D, tex.texture_id);
        m4 proj = m4_proj_ortho(0, w, h, 0, -1, 1);
        glUniformMatrix4fv(glGetUniformLocation(tiles_shader, "uMvp"), 1, GL_FALSE, proj.d);

        GLTiles::vb_clear(vb);

        GameState *gs = get_game_state();
        gs->player_move_input = (v2){};

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            gs->player_move_input.y -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            gs->player_move_input.y += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            gs->player_move_input.x -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            gs->player_move_input.x += 1.0f;
        }

        if (v2_length(gs->player_move_input) > 0.01f)
        {
            gs->player_move_input = v2_normalize(gs->player_move_input);
            // trace("gs->player_move_input = %f, %f", gs->player_move_input.x, gs->player_move_input.y);
        }

        process_input(delta);

        int vert_count = gs->vb->vert_count;
        draw_level();
        int level_verts = gs->vb->vert_count - vert_count;

        vert_count = gs->vb->vert_count;
        draw_player();
        int player_verts = gs->vb->vert_count - vert_count;
        vert_count = gs->vb->vert_count;

        GLTiles::vb_draw_call(vb);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Render Debug");
        ImGui::BulletText("Level verts: %d", level_verts);
        ImGui::BulletText("Player verts: %d", player_verts);
        ImGui::BulletText("Total verts: %d", vert_count);
        ImGui::End();

        window_game_debug();

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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
