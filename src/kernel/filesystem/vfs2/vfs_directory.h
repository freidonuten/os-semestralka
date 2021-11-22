#pragma once

#include "vfs_fat_element.h"
#include "../fat_layer/fat_directory.h"

class VFS_Directory2 : public VFS_Fat_Element2 {	
protected:
	std::shared_ptr<Fat_Directory_Factory> fat_directory_factory;
	std::shared_ptr<Fat_Directory> self_fat_directory;

	std::uint64_t Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, void* buffer, size_t how_many_bytes);

	virtual bool Is_Convertable(std::uint16_t file_attributes);
public:
	VFS_Directory2(std::shared_ptr<Fat_Directory_Factory> factory, char file_name[12], std::uint16_t file_attributes);

	//VFS ELEMENT METHODS
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size);
	virtual void Create();
	virtual bool Remove();
	virtual std::uint64_t Write(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Read(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Seek(std::uint64_t offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation);
	virtual Fat_Dir_Entry Generate_Dir_Entry();

	//DIRECTORY METHODS (to be continued)
};

class VFS_Root_Directory2 : public VFS_Directory2 {
public:
	using VFS_Directory2::VFS_Directory2;

	virtual bool Remove();
};