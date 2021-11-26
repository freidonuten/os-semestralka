#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <vector>
#include <array>
#include <sstream>

/*
	Návratové hodnoty
	0 - Success
	1 - Invalid parameter in rdi
*/

constexpr int freq_table_size = 256;

extern "C" size_t __stdcall freq(const kiv_hal::TRegisters & regs);