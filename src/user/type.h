#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <string.h>

/*
	Návratové hodnoty
	0 - Success
	1 - Invalid parameter in rdi
	2 - File can not be opened
*/

constexpr size_t init_value = 1024;

extern "C" size_t __stdcall type(const kiv_hal::TRegisters & regs);