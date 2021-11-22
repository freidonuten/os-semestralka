#include "vfs_file.h"
#include "../utils/char_utils.h"
#include "../utils/api_utils.h"

VFS_File2::VFS_File2(std::shared_ptr<Fat_File_Factory> fat_file_factory, std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint16_t file_attributes) {
	this->fat_file_factory = fat_file_factory;
	this->parent_fat_directory = parent_directory;
	Char_Utils::Copy_Array(this->file_name, file_name, 12);
	this->file_attributes = file_attributes;
	this->file_position = 0;
}

bool VFS_File2::Is_Convertable(std::uint16_t required_file_attributes) {
	//Can't be directory, can be ro file
	return !api_utils::Check_File_Attributes(required_file_attributes, kiv_os::NFile_Attributes::Directory);
}

void VFS_File2::Create() {
	this->fat_file = this->fat_file_factory->Create_New_File();
}

void VFS_File2::Open(std::uint16_t file_start, std::uint16_t file_size) {
	this->fat_file = this->fat_file_factory->Get_Existing_File(file_start, file_size);
}

bool VFS_File2::Remove() {
	this->fat_file->Remove_File();
	return true;
}

std::uint64_t VFS_File2::Write(size_t how_many_bytes, void* buffer) {
	this->fat_file->Write_To_File(this->file_position, how_many_bytes, buffer);
	this->parent_fat_directory->Change_Entry(this->file_name, this->Generate_Dir_Entry());
	//TODO kolik jsem jich zapsal
	this->file_position += how_many_bytes;
	return how_many_bytes;
}

std::uint64_t VFS_File2::Read(size_t how_many_bytes, void* buffer) {
	this->fat_file->Read_From_File(this->file_position, how_many_bytes, buffer);
	//TODO kolik jsem jich precetl
	this->file_position += how_many_bytes;
	return how_many_bytes;
}

std::uint64_t VFS_File2::Seek(std::uint64_t seek_offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation) {
	std::uint64_t result = static_cast<std::uint64_t>(-1);

	switch (start_position) {
	case kiv_os::NFile_Seek::Beginning:
		this->file_position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		this->file_position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		std::uint64_t file_size = this->fat_file->Get_File_Size();
		this->file_position = file_size;
		break;
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		result = this->file_position;
		break;
	case kiv_os::NFile_Seek::Set_Size:
		this->fat_file->Change_File_Size(this->file_position);
		break;
	case kiv_os::NFile_Seek::Set_Position:
		//DO NOTHING
		break;
	}
	return result;
}

Fat_Dir_Entry VFS_File2::Generate_Dir_Entry() {
	return Fat_Dir_Entry_Factory::Create(this->file_attributes, this->file_name,
		this->fat_file->Get_File_Start(), this->fat_file->Get_File_Size());
}

std::uint64_t VFS_ROFile2::Write(size_t how_many_bytes, void* buffer) {
	//TODO ERROR CANT WRITE TO DIRECTORY - permission denied pres bool
	return 0;
}