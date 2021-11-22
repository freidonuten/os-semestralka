#include "vfs_directory.h"
#include "../utils/char_utils.h"
#include "../utils/api_utils.h"

VFS_Directory2::VFS_Directory2(std::shared_ptr<Fat_Directory_Factory> factory, char file_name[12], std::uint16_t file_attributes) {
	this->fat_directory_factory = factory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
	this->file_position = 0;
}

bool VFS_Directory2::Is_Convertable(std::uint16_t required_file_attributes) {
	if (api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Directory) &&
		api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Read_Only))
	{
		return true;
	}
	return false;
}

void VFS_Directory2::Create() {
	this->self_fat_directory = this->fat_directory_factory->Create_New_Directory();
}

void VFS_Directory2::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->self_fat_directory = this->fat_directory_factory->Get_Existing_Directory(file_start, file_size);
}

bool VFS_Directory2::Remove() {
	bool is_not_empty;
	this->self_fat_directory->Remove_Directory(&is_not_empty);
	return !is_not_empty;
}

std::uint64_t VFS_Directory2::Write(size_t how_many_bytes, void* buffer) {
	//TODO ERROR CANT WRITE TO DIRECTORY - permission denied pres bool
	return 0;
}

std::uint64_t VFS_Directory2::Read(size_t how_many_bytes, void* buffer) {
	auto vector = this->self_fat_directory->Read_All_Entries();
	std::uint64_t bytes_read = Copy_To_TDir_Entry_Format(vector, buffer, how_many_bytes);
	return bytes_read;
}

std::uint64_t VFS_Directory2::Seek(std::uint64_t seek_offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation) {
	std::uint64_t result = static_cast<std::uint64_t>(-1);

	switch (start_position) {
	case kiv_os::NFile_Seek::Beginning:
		this->file_position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		this->file_position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		std::uint64_t file_size = this->self_fat_directory->Get_File_Size();
		this->file_position = file_size;
		break;
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		result = this->file_position;
		break;
	case kiv_os::NFile_Seek::Set_Size:
		//TODO ERROR, CANT CHANGE FILE SIZE
		break;
	case kiv_os::NFile_Seek::Set_Position:
		//DO NOTHING
		break;
	}
	return result;
}

std::uint64_t VFS_Directory2::Copy_To_TDir_Entry_Format(std::vector<Fat_Dir_Entry> entries, void* buffer, size_t max_bytes) {
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

Fat_Dir_Entry VFS_Directory2::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->self_fat_directory->Get_File_Start(), this->self_fat_directory->Get_File_Size());
}

bool VFS_Root_Directory2::Remove() {
	//TODO PERMISSION DENIED
	return false;
}