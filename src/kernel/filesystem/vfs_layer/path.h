#pragma once

#include "vfs_element.h"
#include "../utils/global_constants.h"


#include <memory>



class Filename {
private:
	std::vector<char> implementation;
public:
	void Add_Char(char character);
	void Copy_To_Array(char* result, int count);
	int Get_Length();
	std::shared_ptr<Filename> Copy();
};

class Path {
private:
	std::vector<std::shared_ptr<Filename>> filenames;
	int Append_Buffer(char* buffer, int buffer_index, std::shared_ptr<Filename> filename);
	
	void Make_Filenames(char* path);
public:
	Path(char* path);
	Path();

	int Get_Path_Length();


	std::uint64_t Read_Path(char* buffer, std::uint64_t buffer_size);
	std::uint64_t Read_Path(char* buffer, int filename_count, std::uint64_t buffer_size);
	void Read_Filename(char* buffer);

	std::shared_ptr<VFS_Element> Get_Element(std::shared_ptr<VFS_Element> from);

	std::shared_ptr<Path> Append(std::shared_ptr<Path> end_of_new_path);
	std::shared_ptr<Path> Get_Parent();

	bool Is_Empty();
};


