#include "vfs_directory.h"
#include "../utils/char_utils.h"
#include "../utils/api_utils.h"

VFS_Directory::VFS_Directory(std::shared_ptr<Fat_Directory_Factory> factory, char file_name[12], std::uint8_t file_attributes) {
	this->fat_directory_factory = factory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
	this->file_position = 0;
}

bool VFS_Directory::Is_Convertable(std::uint8_t required_file_attributes) {
	//must be read only directory
	return api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Directory) &&
		api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Read_Only);
}

bool VFS_Directory::Create() {
	auto [element, created] = this->fat_directory_factory->Create_New_Directory();
	if (created) {
		this->self_fat_directory = element;
		return true;
	}
	return false;
}

void VFS_Directory::Open(std::uint16_t file_start, std::uint64_t file_size) {
	this->self_fat_directory = this->fat_directory_factory->Get_Existing_Directory(file_start, file_size);
}

bool VFS_Directory::Remove() {
	return this->self_fat_directory->Remove_Directory();
}

std::uint64_t VFS_Directory::Write(size_t how_many_bytes, void* buffer) {
	return 0;
}

std::uint64_t VFS_Directory::Read(size_t how_many_bytes, void* buffer) {
	auto vector = this->self_fat_directory->Read_All_Entries();
	std::uint64_t bytes_read = Copy_To_TDir_Entry_Format(vector, buffer, how_many_bytes);
	return bytes_read;
}

std::tuple<uint64_t, Seek_Result> VFS_Directory::Seek(std::uint64_t seek_offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation) {
	switch (start_position) {
	case kiv_os::NFile_Seek::Beginning:
		this->file_position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		this->file_position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		this->file_position = this->self_fat_directory->Get_File_Size();
		break;
	default:
		return { 0, Seek_Result::ERROR_INVALID_PARAMETERS };
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		return { this->file_position, Seek_Result::NO_ERROR_POSITION_RETURNED };
	case kiv_os::NFile_Seek::Set_Size:
		return { 0, Seek_Result::ERROR_SETTING_SIZE };
	case kiv_os::NFile_Seek::Set_Position:
		return { 0, Seek_Result::NO_ERROR_POSITION_NOT_RETURNED };
	}

	return { 0, Seek_Result::ERROR_INVALID_PARAMETERS };
}

std::uint64_t VFS_Directory::Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, void* buffer, size_t max_bytes) {
	std::vector<TDir_Entry> temp_vector;
	for (auto entry : entries) {
		TDir_Entry temp_entry;
		temp_entry.file_attributes = entry.file_attributes;
		Char_Utils::Copy_Array(temp_entry.file_name, entry.file_name, 12);
		temp_vector.push_back(temp_entry);
	}

	std::uint64_t size = temp_vector.size() * sizeof(TDir_Entry);
	std::uint64_t to_copy = std::min(max_bytes, size - this->file_position);

	memcpy(buffer, &temp_vector[0], to_copy);
	return to_copy;
}

Fat_Dir_Entry VFS_Directory::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->self_fat_directory->Get_File_Start(), this->self_fat_directory->Get_File_Size());
}

Create_New_Entry_Result VFS_Directory::Create_New_Entry(Fat_Dir_Entry entry) {
	return this->self_fat_directory->Create_New_Entry(entry);
}

std::tuple<Fat_Dir_Entry, bool> VFS_Directory::Read_Entry_By_Name(char file_name[8 + 1 + 3]) {
	return this->self_fat_directory->Read_Entry_By_Name(file_name);
}

bool VFS_Directory::Remove_Entry(char file_name[8 + 1 + 3]) {
	return this->self_fat_directory->Remove_Entry(file_name);
}

bool VFS_Directory::Change_Entry(char old_file_name[8 + 1 + 3], Fat_Dir_Entry new_entry) {
	return this->self_fat_directory->Change_Entry(old_file_name, new_entry);
}

std::shared_ptr<Fat_Directory> VFS_Directory::Get_Fat_Directory() {
	return this->self_fat_directory;
}

bool VFS_Root_Directory::Remove() {
	return false;
}