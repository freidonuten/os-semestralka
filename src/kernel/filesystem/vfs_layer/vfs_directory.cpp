#include "vfs_directory.h"

VFS_Directory::VFS_Directory(VFS_Element_Factory* element_factory, std::shared_ptr<Fat_Directory_Factory> factory,
	std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes) {

	this->element_factory = element_factory;
	this->fat_directory_factory = factory;
	this->parent_fat_directory = parent_directory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;

}

void VFS_Directory::Create() {
	this->self_fat_directory = this->fat_directory_factory->Create_New_Directory();
}

void VFS_Directory::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->self_fat_directory = this->fat_directory_factory->Get_Existing_Directory(file_start, file_size);
}

bool VFS_Directory::Remove() {
	return this->self_fat_directory->Remove_Directory();
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



void VFS_Directory::Change_Size(std::uint32_t desired_size) {
	//TODO ERROR CANT CHANGE SIZE OF DIRECTORY
}

std::shared_ptr<Fat_Dir_Entry> VFS_Directory::Get_ChildO(char file_name[12]) {
	auto result = this->self_fat_directory->Read_Entry_By_Name(file_name);
	auto entry = std::get<0>(result);
	return std::make_shared<Fat_Dir_Entry>(entry);
}

std::shared_ptr<VFS_Element> VFS_Directory::Create_Child(char file_name[12], std::uint16_t file_attributes) {
	auto result = this->element_factory->Create(this->self_fat_directory, file_name, file_attributes);
	result->Create();
	Fat_Dir_Entry entry = result->Generate_Dir_Entry();
	Add_Child(entry);
	return result;
}

std::shared_ptr<VFS_Element> VFS_Directory::Open_Child(char file_name[12]) {
	auto temp = this->self_fat_directory->Read_Entry_By_Name(file_name);
	auto entry = std::get<0>(temp);
	auto result = this->element_factory->Create(this->self_fat_directory, entry.file_name, entry.file_attributes);
	result->Open(entry.file_start, entry.file_size);
	return result;
}

bool VFS_Directory::Contains_Child(char file_name[12]) {
	this->self_fat_directory->Read_Entry_By_Name(file_name);
	return false;
}

void VFS_Directory::Remove_Child(char file_name[12]) {
	auto file = this->Open_Child(file_name);
	if (file->Remove()) {
		this->self_fat_directory->Remove_Entry(file_name);
		Fat_Dir_Entry entry = this->Generate_Dir_Entry();
		this->parent_fat_directory->Change_Entry(this->file_name, entry);
	}
}


void VFS_Directory::Update_ChildO(char old_file_name[12], Fat_Dir_Entry entry) {
	this->self_fat_directory->Change_Entry(old_file_name, entry);
}


Fat_Dir_Entry VFS_Directory::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->self_fat_directory->Get_File_Start(), this->self_fat_directory->Get_File_Size());
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

void VFS_Directory::Add_Child(Fat_Dir_Entry entry) {
	this->self_fat_directory->Create_New_Entry(entry);


		Fat_Dir_Entry self_entry = this->Generate_Dir_Entry();
		this->parent_fat_directory->Change_Entry(this->file_name, self_entry);


}


void Root_Directory::Create(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	this->self_fat_directory = this->fat_directory_factory->Create_New_Directory();
}

bool Root_Directory::Remove(std::shared_ptr<VFS_Directory> parent_vfs_directory) {
	//ERROR CANT REMOVE ROOT DIRECTORY
	return false;
}

void Root_Directory::Add_Child(Fat_Dir_Entry entry) {
	this->self_fat_directory->Create_New_Entry(entry);
}

void Root_Directory::Remove_Child(char file_name[12]) {
	this->self_fat_directory->Remove_Entry(file_name);
}