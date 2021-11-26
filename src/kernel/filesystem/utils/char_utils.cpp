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
		&& string.find("/") == std::string_view::npos
		&& string.compare(".")
		&& string.compare("..");
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