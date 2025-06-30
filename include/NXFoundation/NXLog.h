#pragma once
#include <stdio.h>

#define NXLog(format, ...) \
    fprintf(stderr, (const char* )[format cStr], ##__VA_ARGS__); \
    fprintf(stderr, "\n")
