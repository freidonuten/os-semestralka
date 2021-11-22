#include "vfs_fat_element.h"
#include <stdexcept>

void VFS_Fat_Element2::Close() {
	//EMPTY
}

bool VFS_Fat_Element2::Is_Convertable(std::uint16_t file_attributes) {
	throw std::runtime_error("Unsupported operation IS_CONVERTABLE.");
}

bool VFS_Fat_Element2::Set_File_Attributes(std::uint16_t file_attributes) {
	if (this->Is_Convertable(file_attributes)) {
		this->file_attributes = file_attributes;
		return true;
	}
	else {
		return false;
	}	
}