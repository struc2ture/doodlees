#pragma once

#include "types.hpp"
#include "util.hpp"


#include <cstdlib>

#include <OpenGL/gl3.h>
#include <stb_image.h>

#include "lin_math.cpp"

namespace GLTiles
{
    // Vert buffer
    struct Vert
    {
        f32 x, y;
        f32 u, v;
        f32 fg[4];
        f32 bg[4];
    };
    static Vert make_vert(v2 p, v2 t, v4 fg, v4 bg)
    {
        Vert v;
        v.x = p.x; v.y = p.y;
        v.u = t.x; v.v = t.y;
        v.fg[0] = fg.r; v.fg[1] = fg.g; v.fg[2] = fg.b; v.fg[3] = fg.a;
        v.bg[0] = bg.r; v.bg[1] = bg.g; v.bg[2] = bg.b; v.bg[3] = bg.a;
        return v;
    }

    #define VERT_MAX 65536
    #define INDEX_MAX 131072
    struct Vert_Buf
    {
        Vert verts[VERT_MAX];
        int vert_count;

        u32 indices[INDEX_MAX];
        int index_count;

        GLuint vao, vbo, ebo;
    };

    static size_t vb_vert_size(const Vert_Buf *vb)
    {
        return sizeof(vb->verts[0]) * vb->vert_count;
    }

    static size_t vb_max_vert_size(const Vert_Buf *vb)
    {
        return sizeof(vb->verts[0]) * VERT_MAX;
    }

    static size_t vb_index_size(const Vert_Buf *vb)
    {
        return sizeof(vb->indices[0]) * vb->index_count;
    }

    static size_t vb_max_index_size(const Vert_Buf *vb)
    {
        return sizeof(vb->indices[0]) * INDEX_MAX;
    }

    static Vert_Buf *vb_make()
    {
        Vert_Buf *vb = (Vert_Buf *)malloc(sizeof(Vert_Buf));
        vb->vert_count = 0;
        vb->index_count = 0;

        glGenVertexArrays(1, &vb->vao);
        glGenBuffers(1, &vb->vbo);
        glGenBuffers(1, &vb->ebo);

        glBindVertexArray(vb->vao);
        glBindBuffer(GL_ARRAY_BUFFER, vb->vbo);
        glBufferData(GL_ARRAY_BUFFER, vb_max_vert_size(vb), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vb_max_index_size(vb), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void *)(offsetof(Vert, u)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vert), (void *)(offsetof(Vert, fg)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vert), (void *)(offsetof(Vert, bg)));
        glEnableVertexAttribArray(3);
        glBindVertexArray(0);

        return vb;
    }

    static void vb_clear(Vert_Buf *vb)
    {
        vb->vert_count = 0;
        vb->index_count = 0;
    }

    static void vb_free(Vert_Buf *vb)
    {
        glDeleteBuffers(1, &vb->vbo);
        glDeleteBuffers(1, &vb->ebo);
        glDeleteVertexArrays(1, &vb->vao);
        free(vb);
    }

    static void vb_add_vert(Vert_Buf *vert_buffer, Vert vert)
    {
        if (vert_buffer->vert_count < VERT_MAX)
        {
            vert_buffer->verts[vert_buffer->vert_count++] = vert;
        }
    }

    static int vb_next_vert_index(const Vert_Buf *vb)
    {
        return vb->vert_count;
    }

    static void vb_add_indices(Vert_Buf *vb, int base, int *indices, int index_count)
    {
        for (int i = 0; i < index_count; i++)
        {
            if (vb->index_count < INDEX_MAX)
            {
                vb->indices[vb->index_count++] = base + indices[i];
            }
        }
    }

    static void vb_draw_call(const Vert_Buf *vb)
    {
        glBindVertexArray(vb->vao);
        glBindBuffer(GL_ARRAY_BUFFER, vb->vbo);
        glBufferData(GL_ARRAY_BUFFER, vb_max_vert_size(vb), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vb_vert_size(vb), vb->verts);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vb_max_index_size(vb), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, vb_index_size(vb), vb->indices);

        glDrawElements(GL_TRIANGLES, vb->index_count, GL_UNSIGNED_INT, 0);
    }

    // GL: Shaders
    static bool gl_check_compile_success(GLuint shader, const char *src)
    {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, sizeof(log), NULL, log);

            warning("Shader compile error:\n%s\n\nSource:", log);
            warning("%s", src);
        }
        return (bool)success;
    }

    bool gl_check_link_success(GLuint prog)
    {
        GLint success = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(prog, sizeof(log), NULL, log);

            warning("Program link error:\n%s", log);
        }
        return (bool)success;
    }

    static GLuint gl_create_shader_program(const char *vs_src, const char *fs_src)
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vs_src, 0);
        glCompileShader(vs);
        gl_check_compile_success(vs, vs_src);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fs_src, 0);
        glCompileShader(fs);
        gl_check_compile_success(fs, fs_src);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        gl_check_link_success(prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return prog;
    }

    static GLuint gl_create_tiles_shader()
    {
        const char *vs_src =
            "#version 330 core\n"
            "layout (location = 0) in vec2 inPos;\n"
            "layout (location = 1) in vec2 inTexCoord;\n"
            "layout (location = 2) in vec4 inFgColor;\n"
            "layout (location = 3) in vec4 inBgColor;\n"
            "uniform mat4 uMvp;\n"
            "out vec2 TexCoord;\n"
            "out vec4 FgColor;\n"
            "out vec4 BgColor;\n"
            "void main() {\n"
            "    gl_Position = uMvp * vec4(inPos, 0.0, 1.0);\n"
            "    TexCoord = inTexCoord;\n"
            "    FgColor = inFgColor;\n"
            "    BgColor = inBgColor;\n"
            "}\n";

        const char *fs_src =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoord;\n"
            "in vec4 FgColor;\n"
            "in vec4 BgColor;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    float t =texture(uTex, TexCoord).r;\n"
            "    vec4 c = mix(BgColor, FgColor, t);\n"
            "    FragColor = c;\n"
            // "    FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
            "}\n";

        GLuint prog = gl_create_shader_program(vs_src, fs_src);

        glUseProgram(prog);
        m4 ident = m4_identity();
        glUniformMatrix4fv(glGetUniformLocation(prog, "uMvp"), 1, GL_FALSE, ident.d);
        glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
        glUseProgram(0);

        return prog;
    }

    // GL: Textures
    struct Texture
    {
        GLuint texture_id;
        int w, h, ch;
        GLenum internal_format, format;
    };

    static Texture gl_load_texture(const char *path, GLint sampling_type, bool flip_vertically)
    {
        Texture tex;

        stbi_set_flip_vertically_on_load(flip_vertically);

        unsigned char *tex_data = stbi_load(path, &tex.w, &tex.h, &tex.ch, 0);

        if (tex.ch == 4) { tex.internal_format = GL_RGBA8; tex.format = GL_RGBA; }
        else if (tex.ch == 3) { tex.internal_format = GL_RGB8; tex.format = GL_RGB; }
        else if (tex.ch == 2) { tex.internal_format = GL_RG8; tex.format = GL_RG; }
        else if (tex.ch == 1) { tex.internal_format = GL_R8; tex.format = GL_RED; }

        glGenTextures(1, &tex.texture_id);
        glBindTexture(GL_TEXTURE_2D, tex.texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, tex.w, tex.h, 0, tex.format, GL_UNSIGNED_BYTE, tex_data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling_type);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling_type);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(tex_data);

        return tex;
    }

    static void gl_delete_texture(Texture *tex)
    {
        glDeleteTextures(1, &tex->texture_id);
    }
}
