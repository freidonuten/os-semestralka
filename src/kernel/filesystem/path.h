#pragma once

#include "vfs_element.h"
#include "global_structs.h"

#include <memory>

constexpr auto MAX_FILENAME_SIZE = 12;
constexpr auto MAX_PATH_SIZE = 256;

class Filename {
private:
	std::vector<char> implementation;
public:
	void Add_Char(char character);
	void Copy_To_Array(char* result, int count);
	int Get_Length();
};

class Path {
private:
	std::vector<std::shared_ptr<Filename>> filenames;
	int Append_Buffer(char* buffer, int buffer_index, std::shared_ptr<Filename> filename);
public:
	Path(char* path);

	int Get_Path_Length();
	void Get_Path(char* buffer);
	void Get_Path(char* buffer, int filename_count);

	std::shared_ptr<VFS_Element> Get_Element(std::shared_ptr<VFS_Element> from);
	void Append(char* path);
	std::shared_ptr<Path> Get_Parent();

};


