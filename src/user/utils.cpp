#include "utils.h"
#include "../api/api.h"

#include <array>
#include <string_view>

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

bool utils::is_stop_char(const char c) {
	using kiv_hal::NControl_Codes;

	const auto as_enum_base = static_cast<decltype(NControl_Codes::EOT)>(c);

	return as_enum_base == NControl_Codes::EOT
		|| as_enum_base == NControl_Codes::ETX
		|| as_enum_base == NControl_Codes::SUB;
}


utils::String_View_Tokenizer::String_View_Tokenizer(const std::string_view& source)
	: source(source) {};

std::string_view utils::String_View_Tokenizer::operator()() {
	while (true) {
		if (!source.size()) {
			return {};
		}

		if (source[0] == ' ') {
			source.remove_prefix(1);
			continue;
		}

		if (source[0] == '"') {
				const auto length = source.find_first_of('"', 1);
				if (length == source.npos) {
					break;
				}
				const auto result = std::string_view(source.data() + 1, length - 1);
				source.remove_prefix(length + 1);
				return result;
		}

		const auto length = source.find_first_of(" \"");
		if (length == source.npos) {
			break;
		}
		const auto result = std::string_view(source.data(), length);
		source.remove_prefix(length + (source[length] == ' '));
		return result;
	}

	auto rest = std::string_view();
	rest.swap(source);
	return rest;
}
