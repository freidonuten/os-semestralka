#pragma once

#include "vfs_fat_element.h"

#include "../fat_layer/fat_directory.h"
#include "../fat_layer/fat_file.h"

class VFS_File2 : public VFS_Fat_Element2 {
protected:
	std::shared_ptr<Fat_Directory> parent_fat_directory;
	std::shared_ptr<Fat_File_Factory> fat_file_factory;
	std::shared_ptr<Fat_File> fat_file;

	virtual bool Is_Convertable(std::uint16_t file_attributes);
public:
	VFS_File2(std::shared_ptr<Fat_File_Factory> fat_file_factory, std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes);

	virtual void Open(std::uint16_t file_start, std::uint16_t file_size);
	virtual void Create();
	virtual bool Remove();


	virtual std::uint64_t Write(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Read(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Seek(std::uint64_t offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation);

	virtual Fat_Dir_Entry Generate_Dir_Entry();
};

class VFS_ROFile2 : public VFS_File2 {
public:

	using VFS_File2::VFS_File2;
	virtual std::uint64_t Write(size_t how_many_bytes, void* buffer);
};