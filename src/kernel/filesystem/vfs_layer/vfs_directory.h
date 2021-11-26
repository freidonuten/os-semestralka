#pragma once

#include "vfs_element.h"
#include "../fat_layer/fat_directory.h"
#include "../utils/char_utils.h"
#include "../utils/global_structs.h"

class VFS_Directory : public VFS_Element {
protected:
	std::shared_ptr<Fat_Directory_Factory> fat_directory_factory;
	std::shared_ptr<Fat_Directory> self_fat_directory;

	VFS_Element_Factory* element_factory;
	
	void Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, TDir_Entry* buffer, size_t max_bytes);
	virtual void Add_Child(Fat_Dir_Entry entry);

public:
	VFS_Directory(VFS_Element_Factory* element_factory, std::shared_ptr<Fat_Directory_Factory> factory, std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes);

	virtual void Create();
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size);
	virtual bool Remove();


	virtual int Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	virtual int Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	
	virtual void Change_Size(std::uint32_t desired_size);

	virtual std::shared_ptr<VFS_Element> Create_Child(char file_name[12], std::uint16_t file_attributes);
	virtual std::shared_ptr<VFS_Element> Open_Child(char file_name[12]);
	virtual bool Contains_Child(char file_name[12]);
	virtual void Remove_Child(char file_name[12]);

	virtual std::shared_ptr<Fat_Dir_Entry> Get_ChildO(char file_name[12]);
	virtual void Update_ChildO(char old_file_name[12], Fat_Dir_Entry entry);

	virtual Fat_Dir_Entry Generate_Dir_Entry();


};

class Root_Directory : public VFS_Directory {
protected:
	virtual void Add_Child(Fat_Dir_Entry entry);
public:
	using VFS_Directory::VFS_Directory;

	virtual void Create(std::shared_ptr<VFS_Directory> parent_vfs_directory);
	virtual void Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory);

	
	virtual void Remove_Child(char file_name[12]);
};