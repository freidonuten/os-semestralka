#pragma once

#include "vfs_element.h"
#include "global_structs.h"

#include <memory>

class Path {
private:
	static std::vector<std::array<char, 12>> Parse_Path_To_Filenames(char* path);
	static void Copy_To_Result(char* dest, int dest_i, char* source, int source_i);
	static void Add_To_Vector(std::vector<std::array<char, 12>> result, char current_filename[12]);
public:
	static Element_And_Parent Get_Element_From_Path(std::shared_ptr<VFS_Element> directory, char* path);
};