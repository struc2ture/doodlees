#pragma once

#include <imgui.h>

#include "types.hpp"

#include "gl_tiles.cpp"

#define GAME_COLOR_BLACK (v4){{{0.0f, 0.0f, 0.0f, 1.0f}}}
#define GAME_COLOR_RED   (v4){{{1.0f, 0.0f, 0.0f, 1.0f}}}
#define GAME_COLOR_GRAY1 (v4){{{0.1f, 0.1f, 0.1f, 1.0f}}}
#define GAME_COLOR_GRAY3 (v4){{{0.5f, 0.5f, 0.5f, 1.0f}}}
#define GAME_COLOR_GRAY4 (v4){{{0.7f, 0.7f, 0.7f, 1.0f}}}
#define GAME_COLOR_GRAY5 (v4){{{0.85f, 0.85f, 0.85f, 1.0f}}}
#define GAME_COLOR_ENTITY_BG (v4){{{0.3f, 0.3f, 0.3f, 0.8f}}}

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

struct Glyph
{
    int col, row;
    v4 fg_color, bg_color;
};

Rect get_rect_for_tilemap_glyph(Glyph g)
{
    const int tilemap_rows = 16;
    const int tilemap_cols = 16;
    f32 q_w = 1.0f / tilemap_rows;
    f32 q_h = 1.0f / tilemap_cols;
    Rect result = {
        .min = (v2){{{q_w * g.col, 1.0f - q_h * g.row}}},
        .ext = (v2){{{q_w, -q_h}}}
    };
    return result;
}

enum MapTileKind
{
    MAP_TILE_NONE,
    MAP_TILE_GROUND,
    MAP_TILE_WALL,
    MAP_TILE_COUNT
};

struct MapTile
{
    MapTileKind kind;
    bool is_blocking;
    bool is_opaque;

    Glyph get_glyph()
    {
        switch(kind)
        {
            case MAP_TILE_GROUND: return (Glyph){14, 2, GAME_COLOR_GRAY3, GAME_COLOR_GRAY1};
            case MAP_TILE_WALL:   return (Glyph){3, 2,  GAME_COLOR_GRAY4, GAME_COLOR_GRAY1};
            default:              return (Glyph){0, 0,  GAME_COLOR_RED,   GAME_COLOR_RED};
        }
    }

    const char *get_name()
    {
        switch (kind)
        {
            case MAP_TILE_GROUND: return "Ground";
            case MAP_TILE_WALL: return "Wall";

            case MAP_TILE_NONE:
            case MAP_TILE_COUNT:
                return "Unknown";
        }
    }
};

static inline Glyph get_player_glyph()
{
    return (Glyph){0, 4, GAME_COLOR_GRAY5, GAME_COLOR_ENTITY_BG};
}

f32 get_glyph_dim();

struct Level
{
    static constexpr int COLS = 32;
    static constexpr int ROWS = 32;
    MapTile tiles[ROWS][COLS];

    void set_tile(int col, int row, MapTile tile)
    {
        if (col >= 0 && col < COLS && row >= 0 && row < ROWS)
        {
            tiles[row][col] = tile;
        }
        else
        {
            warning("Out of bounds map access");
        }
    }

    MapTile get_tile(int col, int row)
    {
        if (col >= 0 && col < COLS && row >= 0 && row < ROWS)
        {
            return tiles[row][col];
        }
        else
        {
            warning("Out of bounds map access");
            return (MapTile){MAP_TILE_NONE, false, false};
        }
    }

    MapTile get_tile(v2i p)
    {
        return get_tile(p.x, p.y);
    }

    v2 get_px_size()
    {
        v2 size = {{{COLS * get_glyph_dim(), ROWS * get_glyph_dim()}}};
        return size;
    }

    v2i px_pos_to_tile_pos(v2 px_pos)
    {
        v2i tile_pos = {{{
            truncate_to_int(px_pos.x / get_glyph_dim()),
            truncate_to_int(px_pos.y / get_glyph_dim())
        }}};
        return tile_pos;
    }

    v2i world_pos_to_tile_pos(v2 world_pos)
    {
        v2i tile_pos = {{{
            truncate_to_int(world_pos.x),
            truncate_to_int(world_pos.y)
        }}};
        return tile_pos;
    }

    bool can_move_over_tile(v2i tile_p)
    {
        MapTile tile = get_tile(tile_p);
        return !tile.is_blocking;
    }

    bool can_move_over_tile(v2 world_pos)
    {
        v2i tile_p = world_pos_to_tile_pos(world_pos);
        return can_move_over_tile(tile_p);
    }
};

Level generate_level()
{
    Level level;
    for (int row = 0; row < level.ROWS; row++)
    {
        for (int col = 0; col < level.COLS; col++)
        {
            if (row == 0 || row == level.ROWS - 1 || col == 0 || col == level.COLS - 1)
            {
                level.set_tile(col, row, (MapTile){MAP_TILE_WALL, true, true});
            }
            else
            {
                level.set_tile(col, row, (MapTile){MAP_TILE_GROUND, false, false});
            }
        }
    }
    return level;
}

struct GameState
{
    GLTiles::Vert_Buf *vb;
    v2 player_pos;
    Level level;
    f32 glyph_dim;
    v2 player_move_input;
    bool mouse_left_clicked;
    v2 mouse_pos;
    v2i inspect_tile_pos;
};

static GameState g_GameState;

f32 get_glyph_dim()
{
    return g_GameState.glyph_dim;
}

void game_init(GLTiles::Vert_Buf *vb)
{
    g_GameState.vb = vb;
    g_GameState.level = generate_level();
    g_GameState.player_pos = (v2){{{10.0f, 10.0f}}};
    g_GameState.glyph_dim = 16.0f;
}

GameState *get_game_state()
{
    return &g_GameState;
}

void try_move_player(v2 new_p)
{
    GameState *gs = get_game_state();
    if (gs->level.can_move_over_tile(new_p))
    {
        gs->player_pos = new_p;
    }
}

void process_input(f32 delta)
{
    GameState *gs = get_game_state();
    f32 speed = 4.0f;
    v2 tentative_player_p;
    tentative_player_p.x = gs->player_pos.x + gs->player_move_input.x * delta * speed;
    tentative_player_p.y = gs->player_pos.y + gs->player_move_input.y * delta * speed;
    if (tentative_player_p.x != gs->player_pos.x || tentative_player_p.y != gs->player_pos.y)
        try_move_player(tentative_player_p);

    if (gs->mouse_left_clicked)
    {
        v2 level_px_size = gs->level.get_px_size();
        if (gs->mouse_pos.x < level_px_size.x && gs->mouse_pos.y < level_px_size.y)
        {
            gs->inspect_tile_pos = gs->level.px_pos_to_tile_pos(gs->mouse_pos);
        }
    }
}

void draw_tile(Glyph glyph, Rect screen_pos)
{
    int index_base = GLTiles::vb_next_vert_index(g_GameState.vb);
    v2 a = screen_pos.get_a();
    v2 b = screen_pos.get_b();
    v2 c = screen_pos.get_c();
    v2 d = screen_pos.get_d();

    Rect q = get_rect_for_tilemap_glyph(glyph);
    // trace("glyph rect: %f, %f, %f, %f", q.min.x, q.min.y, q.ext.x, q.ext.y);

    v2 t_a = q.get_a();
    v2 t_b = q.get_b();
    v2 t_c = q.get_c();
    v2 t_d = q.get_d();

    GLTiles::vb_add_vert(g_GameState.vb, GLTiles::make_vert(a, t_a, glyph.fg_color, glyph.bg_color));
    GLTiles::vb_add_vert(g_GameState.vb, GLTiles::make_vert(b, t_b, glyph.fg_color, glyph.bg_color));
    GLTiles::vb_add_vert(g_GameState.vb, GLTiles::make_vert(c, t_c, glyph.fg_color, glyph.bg_color));
    GLTiles::vb_add_vert(g_GameState.vb, GLTiles::make_vert(d, t_d, glyph.fg_color, glyph.bg_color));

    GLTiles::vb_add_indices(g_GameState.vb, index_base, (int[]){0, 1, 3, 1, 2, 3}, 6);
}

void draw_level()
{
    Level *level = &g_GameState.level;
    Rect screen_rect = {
        .min = (v2){{{0.0f, 0.0f}}},
        .ext = (v2){{{get_glyph_dim(), get_glyph_dim()}}}
    };
    for (int row = 0; row < level->ROWS; row++)
    {
        for (int col = 0; col < level->COLS; col++)
        {
            Glyph glyph = level->get_tile(col, row).get_glyph();
            draw_tile(glyph, screen_rect);
            screen_rect.min.x += get_glyph_dim();
        }
        screen_rect.min.x = 0.0f;
        screen_rect.min.y += get_glyph_dim();
    }
}

void draw_player()
{
    Glyph g = get_player_glyph();
    Rect screen_rect = {
        .min = (v2){{{g_GameState.player_pos.x * get_glyph_dim(), g_GameState.player_pos.y * get_glyph_dim()}}},
        .ext = (v2){{{get_glyph_dim(), get_glyph_dim()}}}
    };
    draw_tile(g, screen_rect);
}

void window_game_debug()
{
    GameState *gs = get_game_state();
    ImGui::Begin("Debug");
    ImGui::InputFloat("Player X", &gs->player_pos.x);
    ImGui::InputFloat("Player Y", &gs->player_pos.y);
    ImGui::Separator();
    ImGui::InputFloat("Glyph Dim", &gs->glyph_dim);
    ImGui::SeparatorText("Inspect tile");
    ImGui::BulletText("Pos: %d, %d", gs->inspect_tile_pos.x, gs->inspect_tile_pos.y);
    MapTile tile = gs->level.get_tile(gs->inspect_tile_pos);
    ImGui::BulletText("%s", tile.get_name());
    ImGui::BulletText("Blocking: %s", tile.is_blocking ? "Yes" : "No");
    ImGui::BulletText("Opaque: %s", tile.is_opaque ? "Yes" : "No");
    ImGui::End();

}
