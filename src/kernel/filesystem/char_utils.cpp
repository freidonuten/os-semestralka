#include "char_utils.h"

void Char_Utils::Copy_Array(char* destination, char* source, int count) {
	for (int i = 0; i < count; i++) {
		destination[i] = source[i];
	}
}