#include <stddef.h>
#include <stdlib.h>

#include "common/gl_glue.h"
#include "common/lin_math.h"
#include "common/types.h"
#include "common/util.h"

#define ATLAS_TEXTURE_UNIT 0
#define ATLAS_TEXTURE_UNIT_ENUM (GL_TEXTURE0 + ATLAS_TEXTURE_UNIT)

#define MAX_QUADS 1024
#define MAX_INDICES (MAX_QUADS * 6)
#define VERT_SIZE (sizeof(struct Vert))
#define QUAD_SIZE (sizeof(struct Quad))
#define QUAD_BUF_SIZE (MAX_QUADS * QUAD_SIZE)
#define IND_SIZE (sizeof(ind_buf[0]))
#define IND_BUF_SIZE (MAX_INDICES * IND_SIZE)

#define ATLAS_PATH "res/ui_atlas.png"
#define ATLAS_DIM 1024.0f
#define ATLAS_CELL_DIM 64.0f
#define ATLAS_CELL_PAD 4.0f

struct Vert
{
    v2 pos;
    v2 tex_coord;
    v4 color;
};

struct Quad
{
    struct Vert p[4];
};

globvar GLuint shader_program;
globvar GLint shader_loc_uMvp = 0;
globvar GLint shader_loc_uTex = 0;
globvar GLuint vao, vbo, ebo;

globvar struct Quad quad_buf[MAX_QUADS];
globvar size_t quad_count = 0;

globvar u32 ind_buf[MAX_INDICES];
globvar size_t ind_count = 0;

globvar GLuint atlas_tex;

static const char* vs_src =
    "#version 410 core\n"
    "layout(location = 0) in vec2 inPos;\n"
    "layout(location = 1) in vec2 inTexCoord;\n"
    "layout(location = 2) in vec4 inColor;\n"
    "uniform mat4 uMvp;"
    "out vec2 TexCoord;\n"
    "out vec4 Color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = uMvp * vec4(inPos, 0.0, 1.0);\n"
    "    TexCoord = inTexCoord;\n"
    "    Color = inColor;\n"
    "}\n";

static const char* fs_src =
    "#version 410 core\n"
    "in vec2 TexCoord;\n"
    "in vec4 Color;\n"
    "uniform sampler2D uTex;"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    float t = texture(uTex, vec2(TexCoord.x, TexCoord.y)).r;\n"
    "    FragColor = vec4(Color.rgb, Color.a * t);\n"
    "}\n";

static void _add_indices(u32 base, u32 *indices, int count)
{
    for (int i = 0; i < count; i++)
    {
        ind_buf[ind_count++] = base + indices[i];
    }
}

static void _get_atlas_q_verts(v2i cell_p, v2 out_verts[4])
{
    f32 min_x = cell_p.x * ATLAS_CELL_DIM;
    f32 max_x = min_x + ATLAS_CELL_DIM;
    min_x += ATLAS_CELL_PAD;
    max_x -= ATLAS_CELL_PAD;

    f32 max_y = ATLAS_DIM - cell_p.y * ATLAS_CELL_DIM;
    f32 min_y = max_y - ATLAS_CELL_DIM;
    max_y -= ATLAS_CELL_PAD;
    min_y += ATLAS_CELL_PAD;

    // texel offset, to sample the middle of texels
    min_x += 0.5f;
    max_x += 0.5f;
    min_y += 0.5f;
    max_y += 0.5f;

    // Normalized range
    min_x /= ATLAS_DIM;
    max_x /= ATLAS_DIM;
    min_y /= ATLAS_DIM;
    max_y /= ATLAS_DIM;

    out_verts[0].x = min_x; out_verts[0].y = min_y;
    out_verts[1].x = max_x; out_verts[1].y = min_y;
    out_verts[2].x = max_x; out_verts[2].y = max_y;
    out_verts[3].x = min_x; out_verts[3].y = max_y;
}

void renderer_init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_program = glg__create_shader_program(vs_src, fs_src);

    glUseProgram(shader_program);
    shader_loc_uMvp = glGetUniformLocation(shader_program, "uMvp");
    shader_loc_uTex = glGetUniformLocation(shader_program, "uTex");
    glUniformMatrix4fv(shader_loc_uMvp, 1, GL_FALSE, m4_identity().d);
    glUniform1i(shader_loc_uTex, ATLAS_TEXTURE_UNIT);

    glUseProgram(0);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, QUAD_BUF_SIZE, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, QUAD_BUF_SIZE, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERT_SIZE, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERT_SIZE, (void*)offsetof(struct Vert, tex_coord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VERT_SIZE, (void*)offsetof(struct Vert, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void renderer_init_tex_from_px(void *pixels, int width, int height)
{
    glGenTextures(1, &atlas_tex);
    glActiveTexture(GL_TEXTURE31); // Do texture init on unit 31, to not mess up already setup textures
    glBindTexture(GL_TEXTURE_2D, atlas_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(ATLAS_TEXTURE_UNIT_ENUM);
    glBindTexture(GL_TEXTURE_2D, atlas_tex);
}

void renderer_draw(v2 a, v2 b, v2 c, v2 d, v2 t_a, v2 t_b, v2 t_c, v2 t_d, v4 color)
{
    if (quad_count >= MAX_QUADS)
    {
        warning("Max quad_count reached");
        return;
    }

    int ind_base = quad_count * 4;

    // v2 atlas_q_verts[4];
    // _get_atlas_q_verts(V2I(0, 0), atlas_q_verts);
    v2 atlas_q_verts[4] =
    {
        t_a,
        t_b,
        t_c,
        t_d
    };

    quad_buf[quad_count++] = (struct Quad){
        (struct Vert){a, atlas_q_verts[0], color},
        (struct Vert){b, atlas_q_verts[1], color},
        (struct Vert){c, atlas_q_verts[2], color},
        (struct Vert){d, atlas_q_verts[3], color},
    };

    _add_indices(ind_base, (u32[]){0, 1, 2, 0, 2, 3}, 6);
}

void renderer_render(v2 window_size)
{
    glUseProgram(shader_program);
    m4 proj = m4_proj_ortho(0.0f, window_size.x, window_size.y, 0.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(shader_loc_uMvp, 1, GL_FALSE, proj.d);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, QUAD_BUF_SIZE, NULL, GL_DYNAMIC_DRAW); // orphan
    glBufferSubData(GL_ARRAY_BUFFER, 0, quad_count * QUAD_SIZE, quad_buf);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IND_BUF_SIZE, NULL, GL_DYNAMIC_DRAW); // orphan
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ind_count * IND_SIZE, ind_buf);

    glDrawElements(GL_TRIANGLES, ind_count, GL_UNSIGNED_INT, 0);
    quad_count = 0;
    ind_count = 0;
}
