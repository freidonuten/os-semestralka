#include "vfs_file.h"
#include "char_utils.h"


VFS_File::VFS_File(std::shared_ptr<Fat_File_Factory> fat_file_factory, std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes) {
	this->fat_file_factory = fat_file_factory;
	this->parent_fat_directory = parent_directory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
}

void VFS_File::Create() {
	this->fat_file = this->fat_file_factory->Create_New_File();
}

void VFS_File::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->fat_file = this->fat_file_factory->Get_Existing_File(file_start, file_size);
}

bool VFS_File::Remove() {
	this->fat_file->Remove_File();
	return true;
}

int VFS_File::Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	this->fat_file->Write_To_File(starting_byte, how_many_bytes, buffer);
	this->parent_fat_directory->Change_Entry(this->file_name, this->Generate_Dir_Entry());
	//TODO kolik jsem jich zapsal
	return how_many_bytes;
}

int VFS_File::Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	this->fat_file->Read_From_File(starting_byte, how_many_bytes, buffer);
	return how_many_bytes;
}



void VFS_File::Change_Size(std::uint32_t desired_size) {
	this->fat_file->Change_File_Size(desired_size);
	Fat_Dir_Entry entry = this->Generate_Dir_Entry();
	this->parent_fat_directory->Change_Entry(this->file_name, entry);
}

std::shared_ptr<VFS_Element> VFS_File::Create_Child(char file_name[12], std::uint16_t file_attributes) {
	//TODO ERROR
	return nullptr;
}

std::shared_ptr<VFS_Element> VFS_Directory::Open_Child(char file_name[12]) {
	//TODO ERROR
	return nullptr;
}

void VFS_Directory::Remove_Child(char file_name[12]) {
	//TODO ERROR
}


std::shared_ptr<Fat_Dir_Entry> VFS_File::Get_ChildO(char file_name[12]) {
	//TODO ERROR HANDLER CANT GET CHILD OF FILE
	return nullptr;
}

void VFS_File::Remove_Child(char file_name[12]) {
	//TODO ERROR HANDLER
}

void VFS_File::Update_ChildO(char old_file_name[12], Fat_Dir_Entry entry) {
	//TODO ERROR HANDLER
}


Fat_Dir_Entry VFS_File::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->fat_file->Get_File_Start(), this->fat_file->Get_File_Size());
}