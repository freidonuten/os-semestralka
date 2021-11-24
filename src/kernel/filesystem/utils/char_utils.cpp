#include "char_utils.h"

void Char_Utils::Copy_Array(char* destination, const char* source, int count) {
	for (int i = 0; i < count; i++) {
		destination[i] = source[i];
	}
}

bool utils::Is_Valid_Filename(const char* filename) {
	std::string string(filename);

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