#pragma once

#include <cstdio>

#include "types.hpp"

#define trace(FMT, ...) \
    printf("[TRACE:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define warning(FMT, ...) \
    printf("[WARNING:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define break() __builtin_debugtrap()

#define noop() do {} while (0)

static inline int truncate_to_int(f32 v)
{
    return (int)v;
}
