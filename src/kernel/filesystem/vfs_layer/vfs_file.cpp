#include "vfs_file.h"
#include "../utils/char_utils.h"
#include "../utils/api_utils.h"

VFS_File::VFS_File(std::shared_ptr<Fat_File_Factory> fat_file_factory, std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes) {
	this->fat_file_factory = fat_file_factory;
	this->parent_fat_directory = parent_directory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
	this->file_position = 0;
}

bool VFS_File::Is_Convertable(std::uint16_t required_file_attributes) {
	//Can't be directory, can be ro file
	return !api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Directory);
}

bool VFS_File::Create() {
	auto [element, created] = this->fat_file_factory->Create_New_File();
	if (created) {
		this->fat_file = element;
		return true;
	}
	return false;
}

void VFS_File::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->fat_file = this->fat_file_factory->Get_Existing_File(file_start, file_size);
}

bool VFS_File::Remove() {
	this->fat_file->Remove_File();
	return true;
}

std::uint64_t VFS_File::Write(size_t how_many_bytes, void* buffer) {
	auto written = this->fat_file->Write_To_File(this->file_position, how_many_bytes, buffer);
	this->parent_fat_directory->Change_Entry(this->file_name, this->Generate_Dir_Entry());
	this->file_position += written;
	return written;
}

std::uint64_t VFS_File::Read(size_t how_many_bytes, void* buffer) {
	std::uint64_t read = this->fat_file->Read_From_File(this->file_position, how_many_bytes, buffer);
	this->file_position += read;
	return read;
}

std::tuple<uint64_t, Seek_Result> VFS_File::Seek(std::uint64_t seek_offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation) {
	switch (start_position) {
	case kiv_os::NFile_Seek::Beginning:
		this->file_position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		this->file_position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		this->file_position = this->fat_file->Get_File_Size();
		break;
	default:
		return { 0, Seek_Result::ERROR_INVALID_PARAMETERS };
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		return { this->file_position, Seek_Result::NO_ERROR_POSITION_RETURNED };
	case kiv_os::NFile_Seek::Set_Size:
		this->fat_file->Change_File_Size(this->file_position);
		return { 0, Seek_Result::NO_ERROR_POSITION_NOT_RETURNED };
	case kiv_os::NFile_Seek::Set_Position:
		return { 0, Seek_Result::NO_ERROR_POSITION_NOT_RETURNED };
	}

	return { 0, Seek_Result::ERROR_INVALID_PARAMETERS };
}

Fat_Dir_Entry VFS_File::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->fat_file->Get_File_Start(), this->fat_file->Get_File_Size());
}

std::uint64_t VFS_ROFile::Write(size_t how_many_bytes, void* buffer) {
	return 0;
}