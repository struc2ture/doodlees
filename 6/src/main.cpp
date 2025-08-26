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
}

void on_mouse_cursor(GLFWwindow* window, double xpos, double ypos)
{
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
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

    GLuint tiles_shader = GLTiles::gl_create_tiles_shader();

    GLTiles::Texture tex = GLTiles::gl_load_texture("res/tileset.png", GL_NEAREST, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    f32 delta = 1/120.0f;

    Game game = {};
    game.init();

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

        game.frame(delta);

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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
