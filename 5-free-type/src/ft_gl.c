#include <stdio.h>

#include <freetype/freetype.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "common/types.h"
#include "common/util.h"

#include "font_loader.h"
#include "ppm_write.h"
#include "text_renderer.h"

// MAIN

globvar GLFWwindow *window;

void init()
{
    tr_init();
    tr_init_atlas(font_loader_create_atlas("res/DMMono-Regular.ttf", 512, 512, 32.0f, 2.0f));
}

void frame()
{
    const int starting_ch = 32;
    const int last_ch = 127;

    float pen_x = 5.0f;
    float pen_y = 5.0f;

    const char *text = "LIA, I love you :)\n    - Andrey";
    tr_draw_string(text, &pen_x, &pen_y);

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    tr_render(V2(width, height));
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

        glClearColor(0.68f, 0.26f, 0.26f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        frame();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
