#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <sstream>


/*
	N�vratov� hodnoty
	0 - Success
	1 - File can not be opened
*/

extern "C" size_t __stdcall tasklist(const kiv_hal::TRegisters & regs);