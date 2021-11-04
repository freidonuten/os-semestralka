#pragma once

#include "vfs_element.h"
#include "vfs_directory.h"
#include "fat_file.h"

class VFS_File : public VFS_Element {
private:
	std::shared_ptr<Fat_File_Factory> fat_file_factory;

	std::shared_ptr<Fat_File> fat_file;
	char file_name[12];
public:
	VFS_File(std::shared_ptr<Fat_File_Factory> fat_file_factory, std::shared_ptr<VFS_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes);

	virtual void Create(std::shared_ptr<VFS_Directory> parent_vfs_directory);
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size);
	virtual int Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	virtual int Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	virtual void Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory);
	virtual void Change_Size(std::uint32_t desired_size);

	virtual std::shared_ptr<Fat_Dir_Entry> Get_Child(char file_name[12]);
	virtual void Add_Child(Fat_Dir_Entry entry);
	virtual void Remove_Child(char file_name[12]);
	virtual void Update_Child(char old_file_name[12], Fat_Dir_Entry entry);

	virtual Fat_Dir_Entry Generate_Dir_Entry();
};