#pragma once

#include "../api/api.h"
#include "global.h"

#include <time.h>
#include <string>

constexpr int eof_buffer_size = 1;

extern "C" size_t __stdcall rgen(const kiv_hal::TRegisters & regs);
size_t Terminated(const kiv_hal::TRegisters& regs);
size_t Check_EOF(const kiv_hal::TRegisters& regs);