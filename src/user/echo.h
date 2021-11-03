#pragma once

#include "../api/api.h"
#include "rtl.h"
#include <string>
#include <string_view>

/*
	Návratové hodnoty
	0 - Success
*/

bool is_echo_on = true;
constexpr std::string_view ECHO_ON = "on";
constexpr std::string_view ECHO_OFF = "off";
constexpr std::string_view AT_SIGN = "@";
constexpr std::string_view ECHO_IS_ON = "Echo is On\n";
constexpr std::string_view ECHO_IS_OFF = "Echo is Off\n";

extern "C" size_t __stdcall echo(const kiv_hal::TRegisters & regs);