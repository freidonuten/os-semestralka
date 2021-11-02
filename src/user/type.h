#pragma once

#include "../api/api.h"
#include "rtl.h"
#include "global.h"
#include <string.h>

constexpr size_t init_value = 1024;
constexpr kiv_os::THandle invalid_file_handle = static_cast<kiv_os::THandle>(-1);

extern "C" size_t __stdcall type(const kiv_hal::TRegisters & regs);