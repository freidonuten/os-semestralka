#include "utils.h"
#include "../api/api.h"

#include <array>

static constexpr std::array<std::string_view, 10> messages{
	std::string_view{"Unknown error.\n"},
	std::string_view{"Success.\n"},
	std::string_view{"Unknown filesystem.\n"},
	std::string_view{"Invalid argument.\n"},
	std::string_view{"File not found.\n"},
	std::string_view{"Directory not empty.\n"},
	std::string_view{"Not enough disk space.\n"},
	std::string_view{"Out of memory.\n"},
	std::string_view{"Permission denied.\n"},
	std::string_view{"IO Error.\n"},
};

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

std::string_view utils::get_error_message(kiv_os::NOS_Error error) {
	std::uint16_t index = static_cast<uint16_t>(error) + 1;
	return messages[index];
}

