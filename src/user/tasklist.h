#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <sstream>


/*
	Návratové hodnoty
	0 - Success
	1 - File can not be opened
*/

constexpr std::string_view TASKLIST_TITLE = "PID\tName\n";

extern "C" size_t __stdcall tasklist(const kiv_hal::TRegisters & regs);