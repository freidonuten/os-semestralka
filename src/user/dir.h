#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <vector>
#include <sstream>

/*
	Návratové hodnoty
	0 - Success
	1 - Invalid parameter in rdi
	2 - File can not be opened
*/

constexpr size_t dir_size = sizeof(kiv_os::TDir_Entry);
constexpr uint16_t dir_type = static_cast<uint16_t>(kiv_os::NFile_Attributes::Directory);
constexpr uint16_t file_type = static_cast<uint16_t>(kiv_os::NFile_Attributes::System_File);


extern "C" size_t __stdcall dir(const kiv_hal::TRegisters & regs);