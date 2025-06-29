#pragma once
#include <stdio.h>

#define NSLog(format, ...) \
    fprintf(stderr, [format cStr], ##__VA_ARGS__); \
    fprintf(stderr, "\n")
