#pragma once

#include <cstdio>

#include "types.hpp"

#define trace(FMT, ...) \
    printf("[TRACE:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define warning(FMT, ...) \
    printf("[WARNING:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define break() __builtin_debugtrap()

#define bp() __builtin_debugtrap()
#define noop() do {} while (0)

#define array_size(ARR) (sizeof((ARR))/sizeof((ARR[0])))

#define STR_BUF_SMALL 64
#define STR_BUF_MED 256
#define STR_BUF_LARGE 1024

static inline int truncate_to_int(f32 v)
{
    return (int)v;
}
