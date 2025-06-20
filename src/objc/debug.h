#pragma once
#include <stdlib.h>

void hexdump(void* ptr,size_t size);
void debugin();
void debugf(const char* format, ...);
void debugout();
