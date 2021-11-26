#include "api_utils.h"

bool api_utils::Check_File_Attributes(std::uint16_t file_attributes, kiv_os::NFile_Attributes attribute) {
	return Bit_Utils::Check_Individual_Bit(file_attributes, static_cast<uint8_t>(attribute));
}