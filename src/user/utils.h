#pragma once

#include <cstdint>

namespace utils {
	uint8_t get_dir_attrs();
	uint8_t get_file_attrs();
	bool is_dir(uint8_t attrs);
	bool is_file(uint8_t attrs);
}