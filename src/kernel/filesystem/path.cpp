#include "path.h"



std::vector<std::array<char, 12>> Path::Parse_Path_To_Filenames(char* path) {
	std::vector<std::array<char, 12>> result;
	char current_filename[12];
	int path_index = 0;
	int filename_index = 0;	

	while (true) {
		switch (path[path_index]) {
			case 0:
				break;
			case '/':
				break;
			default:
				Copy_To_Result(current_filename, filename_index, path, path_index);
				filename_index++;
				break;
		}
		path_index++;
	}
}

void Path::Copy_To_Result(char* dest, int dest_i, char* source, int source_i) {
	if (dest_i >= 11) {
		//TODO ERROR_HANDLER
		return;
	}
	dest[dest_i] = source[source_i];
}

void Add_To_Vector(std::vector<std::array<char, 12>> result, char current_filename[12]) {
	std::array<char, 12> to_add = current_filename

}