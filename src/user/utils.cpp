#include "utils.h"

#include "../api/api.h"

uint8_t utils::get_dir_attrs() {
	uint8_t directory = static_cast<uint8_t>(kiv_os::NFile_Attributes::Directory);
	uint8_t read_only = static_cast<uint8_t>(kiv_os::NFile_Attributes::Read_Only);
	return directory + read_only;
}

uint8_t utils::get_file_attrs() {
	return 0;
}

bool utils::is_dir(uint8_t attrs) {
	uint8_t directory = static_cast<uint8_t>(kiv_os::NFile_Attributes::Directory);
	uint8_t mask_result = directory & attrs;
	return mask_result != 0;
}

bool utils::is_file(uint8_t attrs) {
	return !utils::is_dir(attrs);
}