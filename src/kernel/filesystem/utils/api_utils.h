#pragma once

#include "../../../api/api.h"
#include "bit_utils.h"

namespace api_utils {

	bool Check_File_Attributes(std::uint8_t file_attributes, kiv_os::NFile_Attributes attribute);
}