#pragma once

#include "disk.h"
#include "filesystem_info.h"
#include "fat_file.h"
#include "fat_directory.h"
#include "file_descriptor.h"

#include <memory>

class Actions {
private:
	std::shared_ptr<IDisk> disk;
	std::shared_ptr<Filesystem_Info> info;
	std::shared_ptr<Fat_File_Factory> file_factory;
	std::shared_ptr<Fat_Directory_Factory> directory_factory;
	std::shared_ptr<File_Descriptor_Table> descriptor_table;

	void Add_Entry_To_Parent(std::shared_ptr<Fat_Directory> parent, std::shared_ptr<Fat_File> file, char file_name[12], std::uint16_t attributes);
public:
	Actions();

	std::shared_ptr<File_Descriptor> Create_New_File(std::shared_ptr<Fat_Directory> parent, char file_name[12], std::uint16_t attributes);
	std::shared_ptr<File_Descriptor> Open_File(std::shared_ptr<Fat_Directory> parent, char file_name[12]);
	void Write_To_File(std::uint16_t file_descriptor);



};