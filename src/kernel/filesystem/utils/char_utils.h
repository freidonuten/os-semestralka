#pragma once

#include <string>
#include "global_structs.h"

class Char_Utils {
public:
	static void Copy_Array(char* destination, const char* source, int count);
};

namespace utils {
	bool Is_Valid_Filename(const char* filename);
	Path_Type Get_Path_Type(const char* path);
	Filename_Type Get_Filename_Type(const char* filename);
}