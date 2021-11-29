#include "vfs_fat_element_factory.h"

VFS_Fat_Element_Factory::VFS_Fat_Element_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->file_factory = std::make_shared<Fat_File_Factory>(disk, info);
	this->directory_factory = std::make_shared<Fat_Directory_Factory>(this->file_factory);
}

std::shared_ptr<VFS_Fat_Element> VFS_Fat_Element_Factory::Make(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes) {
	VFS_Fat_Element_Type element_type = this->Get_Element_Type(file_attributes);

	switch (element_type) {
	case VFS_Fat_Element_Type::DIRECTORY:
		return std::make_shared<VFS_Directory>(this->directory_factory, parent_directory, file_name, file_attributes);
	case VFS_Fat_Element_Type::FILE:
		return std::make_shared<VFS_File>(this->file_factory, parent_directory, file_name, file_attributes);
	case VFS_Fat_Element_Type::ROFILE:
		return std::make_shared<VFS_ROFile>(this->file_factory, parent_directory, file_name, file_attributes);
	default:
		return nullptr;
	}
}

std::shared_ptr<VFS_Directory> VFS_Fat_Element_Factory::Make_Root_Directory() {
	char filename[12] = "";
	std::shared_ptr<VFS_Directory> result = std::make_shared<VFS_Root_Directory>(
		this->directory_factory, nullptr, filename, 0xff);

	return result;
}

VFS_Fat_Element_Type VFS_Fat_Element_Factory::Get_Element_Type(std::uint8_t file_attributes) {
	bool is_directory = api_utils::Check_File_Attributes(file_attributes, kiv_os::NFile_Attributes::Directory);
	bool is_read_only = api_utils::Check_File_Attributes(file_attributes, kiv_os::NFile_Attributes::Read_Only);
	
	if (is_directory && is_read_only) {
		return VFS_Fat_Element_Type::DIRECTORY;
	}

	if (!is_directory && is_read_only) {
		return VFS_Fat_Element_Type::ROFILE;
	}

	if (!is_directory && !is_read_only) {
		return VFS_Fat_Element_Type::FILE;
	}

	return VFS_Fat_Element_Type::INVALID;
}