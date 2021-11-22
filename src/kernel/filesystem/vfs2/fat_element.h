#pragma once

#include "vfs_element.h"
#include "../fat_layer/fat_directory.h"

class VFS_Fat_Element2 : public VFS_Element2 {
protected:
	std::uint16_t file_attributes;
	char file_name[12]; 
	std::uint64_t file_position;

	virtual bool Is_Convertable(std::uint16_t file_attributes);
public:
	virtual void Close();
	virtual bool Set_File_Attributes(std::uint16_t file_attributes);
};