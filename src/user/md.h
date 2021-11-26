#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"

/*
	Návratové hodnoty
	0 - Success
	1 - Invalid parameter in rdi
	2 - File can not be opened
*/

extern "C" size_t __stdcall md(const kiv_hal::TRegisters & regs);