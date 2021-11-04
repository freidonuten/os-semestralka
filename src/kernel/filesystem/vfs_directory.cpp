#include "vfs_directory.h"

VFS_Directory::VFS_Directory(std::shared_ptr<Fat_Directory_Factory> factory,
	std::shared_ptr<VFS_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes) {

	this->fat_directory_factory = factory;
	this->parent_fat_directory = parent_directory == nullptr ? nullptr : parent_directory->Get_Fat_Directory(); //if its root, it does not have parent
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
}

void VFS_Directory::Create(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	this->self_fat_directory = this->fat_directory_factory->Create_New_Directory();
	Fat_Dir_Entry entry = this->Generate_Dir_Entry();
	parent_vfs_directory->Add_Child(entry);
}

void VFS_Directory::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->self_fat_directory = this->fat_directory_factory->Get_Existing_Directory(file_start, file_size);
}

int VFS_Directory::Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	//TODO ERROR CANT WRITE TO DIRECTORY
	return 0;
}

int VFS_Directory::Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	auto vector = this->self_fat_directory->Read_All_Entries();
	Copy_To_TDir_Entry_Format(vector, static_cast<TDir_Entry*>(buffer), how_many_bytes);
	return how_many_bytes;
}

void VFS_Directory::Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	if (this->self_fat_directory->Get_File_Size() != 0) {
		//TODO ERROR NOT EMPTY
		return;
	}

	parent_vfs_directory->Remove_Child(this->file_name);
	this->self_fat_directory->Remove_Directory();
}

void VFS_Directory::Change_Size(std::uint32_t desired_size) {
	//TODO ERROR CANT CHANGE SIZE OF DIRECTORY
}

std::shared_ptr<Fat_Dir_Entry> VFS_Directory::Get_Child(char file_name[12]) {
	auto result = this->self_fat_directory->Read_Entry_By_Name(file_name);
	return std::make_shared<Fat_Dir_Entry>(result);
}

void VFS_Directory::Add_Child(Fat_Dir_Entry entry) {
	this->self_fat_directory->Create_New_Entry(entry);
	Fat_Dir_Entry self_entry = this->Generate_Dir_Entry();
	this->parent_fat_directory->Change_Entry(this->file_name, self_entry);
}

void VFS_Directory::Remove_Child(char file_name[12]) {
	this->self_fat_directory->Remove_Entry(file_name);
	Fat_Dir_Entry entry = this->Generate_Dir_Entry();
	this->parent_fat_directory->Change_Entry(this->file_name, entry);
}

void VFS_Directory::Update_Child(char old_file_name[12], Fat_Dir_Entry entry) {
	this->self_fat_directory->Change_Entry(old_file_name, entry);
}


Fat_Dir_Entry VFS_Directory::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->self_fat_directory->Get_File_Start(), this->self_fat_directory->Get_File_Size());
}

std::shared_ptr<Fat_Directory> VFS_Directory::Get_Fat_Directory() {
	return this->self_fat_directory;
}

void VFS_Directory::Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, TDir_Entry* buffer, size_t max_bytes) {
	int entries_size = entries.size();
	int buffer_size = max_bytes / sizeof(TDir_Entry);
	int count = std::min(entries_size, buffer_size);

	for (int i = 0; i < count; i++) {
		Char_Utils::Copy_Array(buffer[i].file_name, entries[i].file_name, 12);
		buffer[i].file_attributes = entries[i].file_attributes;
	}
}

void Root_Directory::Create(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	this->self_fat_directory = this->fat_directory_factory->Create_New_Directory();
}

void Root_Directory::Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	if (this->self_fat_directory->Get_File_Size() != 0) {
		//TODO ERROR NOT EMPTY
		return;
	}

	this->self_fat_directory->Remove_Directory();
}

void Root_Directory::Add_Child(Fat_Dir_Entry entry) {
	this->self_fat_directory->Create_New_Entry(entry);
}

void Root_Directory::Remove_Child(char file_name[12]) {
	this->self_fat_directory->Remove_Entry(file_name);
}