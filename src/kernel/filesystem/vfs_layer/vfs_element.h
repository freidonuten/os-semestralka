#pragma once

#include "../utils/global_structs.h"
#include "../fat_layer/fat_directory.h"
#include "../fat_layer/fat_file.h"

#include "../../../api/api.h"

#include <cstdint>
#include <memory>

class VFS_Directory; //forward declaration

class VFS_Element {
protected:
	std::uint16_t file_attributes;
	char file_name[12];
	std::shared_ptr<Fat_Directory> parent_fat_directory;
public:
	//IS CALLED BY PARENT DIRECTORY
	virtual void Create() = 0;
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size) = 0;
	virtual bool Remove() = 0;

	//OPERATIONS CALLED BY ACTIONS
	virtual int Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) = 0;
	virtual int Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) = 0;
	virtual void Change_Size(std::uint32_t desired_size) = 0;
	virtual void Change_Attributes(std::uint16_t file_attributes);
	virtual std::uint16_t Read_Attributes();
	virtual char* Read_File_Name();

	//DIRECTORY ACTIONS
	virtual std::shared_ptr<VFS_Element> Create_Child(char file_name[12], std::uint16_t file_attributes) = 0;
	virtual std::shared_ptr<VFS_Element> Open_Child(char file_name[12]) = 0;
	virtual bool Contains_Child(char file_name[12]) = 0;
	virtual void Remove_Child(char file_name[12]) = 0;
	
	virtual std::shared_ptr<Fat_Dir_Entry> Get_ChildO(char file_name[12]) = 0;
	virtual void Update_ChildO(char old_file_name[12], Fat_Dir_Entry entry) = 0;


	//GENERATE PHYSICAL REPRESENTATION OF ENTRY IN FOLDER ON DISK
	virtual Fat_Dir_Entry Generate_Dir_Entry() = 0;

};

class VFS_Element_Factory {
private:
	std::shared_ptr<Fat_File_Factory> file_factory;
	std::shared_ptr<Fat_Directory_Factory> directory_factory;

	bool Is_Directory(std::uint8_t file_attributes);

public:
	VFS_Element_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::shared_ptr<VFS_Element> Create(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes);
	std::shared_ptr<VFS_Element> Create_Root_Directory();
};