#pragma once

#include "../api/api.h"
#include "rtl.h"

/*
	Návratové hodnoty
	0 - Success
*/

extern "C" size_t __stdcall shutdown(const kiv_hal::TRegisters & regs);