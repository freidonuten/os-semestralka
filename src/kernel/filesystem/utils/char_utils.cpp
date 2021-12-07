#include "char_utils.h"

void Char_Utils::Copy_Array(char* destination, const char* source, int count) {
	for (int i = 0; i < count; i++) {
		destination[i] = source[i];
	}
}

bool utils::Is_Valid_Filename(const char* filename) {
	const auto string = std::string_view(filename);

	// has non-zero size, doesn't contain slash and isn't dot or double dot
	return string.size()
		&& string.find("\\") == std::string_view::npos
		&& string.compare(".")
		&& string.compare("..");
}

Path_Type utils::Get_Path_Type(const char* path) {
	Filename_Type filename_type = utils::Get_Filename_Type(path);

	switch (filename_type) {
		case Filename_Type::INVALID:			return Path_Type::INVALID;
		case Filename_Type::ABSOLUTE_PATH:		return Path_Type::ABSOLUTE_PATH;
	}

	return Path_Type::RELATIVE_PATH;
}

Filename_Type utils::Get_Filename_Type(const char* filename) {
	const auto string = std::string_view(filename);

	if (!string.size()) {
		return Filename_Type::INVALID;
	}

	if (string.compare(".") == 0) {
		return Filename_Type::CURRENT;
	}

	if (string.compare("..") == 0) {
		return Filename_Type::RELATIVE_PATH;
	}

	if (string.find("\\") == std::string_view::npos) {
		return Filename_Type::FILENAME;
	}

	if (string[0] == '\\') {
		return Filename_Type::ABSOLUTE_PATH;
	}

	return Filename_Type::RELATIVE_PATH;
}