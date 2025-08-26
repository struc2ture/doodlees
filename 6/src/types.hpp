#pragma once

#pragma once

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;

struct v2
{
    union
    {
        struct { f32 x; f32 y; };
        struct { f32 d[2]; };
    };
};

struct v3
{
    union
    {
        struct { f32 x; f32 y; f32 z; };
        struct { f32 r; f32 g; f32 b; };
        struct { f32 d[3]; };
    };
};

struct v4
{
    union
    {
        struct { f32 x; f32 y; f32 z; f32 w; };
        struct { f32 r; f32 g; f32 b; f32 a; };
        struct { f32 d[4]; };
    };
};

struct v2i
{
    union
    {
        struct { i32 x; i32 y; };
        struct { i32 d[2]; };
    };
};

struct v3i
{
    union
    {
        struct { i32 x; i32 y; i32 z; };
        struct { i32 r; i32 g; i32 b; };
        struct { i32 d[3]; };
    };
};

struct v4i
{
    union
    {
        struct { i32 x; i32 y; i32 z; i32 w; };
        struct { i32 r; i32 g; i32 b; i32 a; };
        struct { i32 d[4]; };
    };
};

/*
 * OpenGL expects column-major.
 * m[column][row]
 * m[column * 4 + row]
 * M00 M01 M02 M03
 * M10 M11 M12 M13
 * M20 M21 M22 M23
 * M30 M31 M32 M33
 *
 * Laid out in memory like this:
 * M00 M10 M20 M30 M01 M11 ...
 */
struct m4
{
    f32 d[16];
};
