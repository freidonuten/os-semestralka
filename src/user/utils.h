#pragma once

#include <cstdint>
#include <string_view>
#include "../api/api.h"

namespace utils {
	uint8_t get_dir_attrs();
	uint8_t get_file_attrs();
	bool is_dir(uint8_t attrs);
	bool is_file(uint8_t attrs);
	std::string_view get_error_message(kiv_os::NOS_Error error);
}