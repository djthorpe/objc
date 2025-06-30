#pragma once
#include <stdio.h>

#define NXLog(format, ...) \
    fprintf(stderr, [format cStr], ##__VA_ARGS__); \
    fprintf(stderr, "\n")
