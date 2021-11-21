#pragma once

#include "vfs_element.h"
#include "../fat_layer/fat_directory.h"

class Fat_Element2 : public VFS_Element2 {
protected:
	std::uint16_t file_attributes;
	char file_name[12];
	std::uint64_t file_position;

	std::shared_ptr<Fat_Directory> parent_fat_directory;

	virtual bool Is_Convertable(std::uint16_t file_attributes);
public:
	virtual void Close();
	virtual bool Set_File_Attributes(std::uint16_t file_attributes);
};