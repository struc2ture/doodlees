#include <stdio.h>

#include <freetype/freetype.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#define trace(FMT, ...) \
    printf("[TRACE:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define globvar static

static inline int truncate_to_int(float v)
{
    return (int)v;
}

static inline float convert_26dot6_float(FT_Pos x)
{
    return (float)(x / 64.0f);
}

// MAIN

globvar GLFWwindow *window;

void init()
{

}

void frame()
{
        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        glViewport(0, 0, fb_w, fb_h);

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        glClearColor(0.18f, 0.86f, 0.26f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

        frame();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
