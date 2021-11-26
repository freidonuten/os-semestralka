#include "char_utils.h"

void Char_Utils::Copy_Array(char* destination, const char* source, int count) {
	for (int i = 0; i < count; i++) {
		destination[i] = source[i];
	}
}

bool utils::Is_Valid_Filename(const char* filename) {
	const auto string = std::string_view(filename);

	if (string.size() == 0) {
		return false;
	}

	if (string.find("/") != std::string::npos) {
		return false;
	}

	if (string.compare(".") == 0) {
		return false;
	}

	if (string.compare("..") == 0) {
		return false;
	}

	return true;
}

Path_Type utils::Get_Path_Type(const char* path) {
	if (path[0] == 0) { //EMPTY ""
		return Path_Type::INVALID;
	}

	if (path[0] == '/') {
		return Path_Type::ABSOLUTE_PATH;
	}

	return Path_Type::RELATIVE_PATH;
}