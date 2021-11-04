#pragma once

#include "global_structs.h"
#include "fat_directory.h"
#include "fat_file.h"

#include "../../api/api.h"

#include <cstdint>
#include <memory>

class VFS_Directory; //forward declaration

class VFS_Element {
protected:
	std::uint16_t file_attributes;
	char file_name[12];
	std::shared_ptr<Fat_Directory> parent_fat_directory;
public:
	virtual void Create(std::shared_ptr<VFS_Directory> parent_vfs_directory) = 0;
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size) = 0;
	virtual int Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) = 0;
	virtual int Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) = 0;
	virtual void Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory) = 0;
	virtual void Change_Attributes(std::uint16_t file_attributes);
	virtual std::uint16_t Read_Attributes();
	virtual void Change_Size(std::uint32_t desired_size) = 0;

	virtual std::shared_ptr<Fat_Dir_Entry> Get_Child(char file_name[12]) = 0;
	virtual void Add_Child(Fat_Dir_Entry entry) = 0;
	virtual void Remove_Child(char file_name[12]) = 0;
	virtual void Update_Child(char old_file_name[12], Fat_Dir_Entry entry) = 0;

	virtual Fat_Dir_Entry Generate_Dir_Entry() = 0;

};

class VFS_Element_Factory {
private:
	std::shared_ptr<Fat_File_Factory> file_factory;
	std::shared_ptr<Fat_Directory_Factory> directory_factory;

	bool Is_Directory(std::uint8_t file_attributes);

public:
	VFS_Element_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::shared_ptr<VFS_Element> Create_New(std::shared_ptr<VFS_Element> parent_directory, char file_name[12], std::uint8_t file_attributes);
	std::shared_ptr<VFS_Element> Get_Existing(std::shared_ptr<VFS_Element> parent_directory, char file_name[12]);
	std::shared_ptr<VFS_Element> Create_Root_Directory();
};