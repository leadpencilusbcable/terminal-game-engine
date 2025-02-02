#pragma once

#include "stdbool.h"
#include <stdint.h>

#define RESET "\033[0m"

#define RED(x) "\033[31m" x RESET
#define GRN(x) "\033[32m" x RESET

bool expect_int(int value, int expected, char* desc);
bool expect_uint(unsigned int value, unsigned int expected, char* desc);
bool expect_uint64_t(uint64_t value, uint64_t expected, char* desc);

