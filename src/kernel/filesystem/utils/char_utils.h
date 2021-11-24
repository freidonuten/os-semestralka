#pragma once

#include <string>

class Char_Utils {
public:
	static void Copy_Array(char* destination, const char* source, int count);
};

namespace utils {
	bool Is_Valid_Filename(const char* filename);
}