#pragma once

#include "vfs_element.h"
#include "fat_directory.h"
#include "char_utils.h"
#include "global_structs.h"

class VFS_Directory : public VFS_Element {
protected:
	std::shared_ptr<Fat_Directory_Factory> fat_directory_factory;

	std::shared_ptr<Fat_Directory> self_fat_directory;
	
	void Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, TDir_Entry* buffer, size_t max_bytes);

public:
	VFS_Directory(std::shared_ptr<Fat_Directory_Factory> factory, std::shared_ptr<VFS_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes);

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

	std::shared_ptr<Fat_Directory> Get_Fat_Directory();

};

class Root_Directory : public VFS_Directory {
public:
	using VFS_Directory::VFS_Directory;

	virtual void Create(std::shared_ptr<VFS_Directory> parent_vfs_directory);
	virtual void Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory);

	virtual void Add_Child(Fat_Dir_Entry entry);
	virtual void Remove_Child(char file_name[12]);
};