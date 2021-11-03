#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"

/*
	Návratové hodnoty
	0 - Success
	1 - Invalid parameter in rdi
*/

extern "C" size_t __stdcall rd(const kiv_hal::TRegisters & regs);