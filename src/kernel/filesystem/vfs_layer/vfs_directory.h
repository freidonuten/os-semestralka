#pragma once

#include "vfs_fat_element.h"
#include "../fat_layer/fat_directory.h"

class VFS_Directory : public VFS_Fat_Element {	
protected:
	std::shared_ptr<Fat_Directory_Factory> fat_directory_factory;
	std::shared_ptr<Fat_Directory> self_fat_directory;
	std::shared_ptr<Fat_Directory> parent_fat_directory;

	std::uint64_t Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, void* buffer, size_t how_many_bytes);

	virtual bool Is_Convertable(std::uint8_t file_attributes);
public:
	VFS_Directory(std::shared_ptr<Fat_Directory_Factory> factory, std::shared_ptr<Fat_Directory> parent_fat_directory, char file_name[12], std::uint8_t file_attributes);

	//VFS ELEMENT METHODS
	virtual void Open(std::uint16_t file_start, std::uint64_t file_size);
	virtual bool Create();
	virtual bool Remove();
	virtual std::uint64_t Write(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Read(size_t how_many_bytes, void* buffer);
	virtual std::tuple<uint64_t, Seek_Result> Seek(std::uint64_t offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation);
	virtual Fat_Dir_Entry Generate_Dir_Entry();

	//DIRECTORY METHODS (wrappers)
	virtual Create_New_Entry_Result Create_New_Entry(Fat_Dir_Entry entry); //out => true = ok, false = already_exists
	std::tuple<Fat_Dir_Entry, bool> Read_Entry_By_Name(char file_name[8 + 1 + 3]);
	bool Remove_Entry(char file_name[8 + 1 + 3]); //out => true = ok, false = not_found
	bool Change_Entry(char old_file_name[8 + 1 + 3], Fat_Dir_Entry new_entry); //out => true = ok, false = not_found
	std::shared_ptr<Fat_Directory> Get_Fat_Directory();
};

class VFS_Root_Directory : public VFS_Directory {
public:
	using VFS_Directory::VFS_Directory;

	virtual bool Remove();
	virtual Create_New_Entry_Result Create_New_Entry(Fat_Dir_Entry entry); //out => true = ok, false = already_exists
};