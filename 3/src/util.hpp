#pragma once

#include <cstdio>

#define trace(FMT, ...) \
    printf("[TRACE:%s:%d(%s)] " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define break() __builtin_debugtrap()

#define noop() do {} while (0)
