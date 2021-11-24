#include "file_descriptor.h"

std::uint16_t File_Descriptor_Table::Create_Descriptor(std::shared_ptr<VFS_Element> element) {
	int count = this->descriptors.size();
	for (std::uint16_t i = 0; i < count; i++) {
		if (this->descriptors[i] == nullptr) {
			this->descriptors[i] = element;
			return i;
		}
	}

	this->descriptors.push_back(element);
	return count;
}

std::shared_ptr<VFS_Element> File_Descriptor_Table::Get_Descriptor(std::uint16_t id) {
	if (Is_Valid(id)) {
		return this->descriptors[id];
	}
	
	//TODO Error Handler descriptor not existing
}

void File_Descriptor_Table::Remove_Descriptor(std::uint16_t id) {
	if (Is_Valid(id)) {
		this->descriptors[id] = nullptr;
	}
	//TODO Error Handler descriptor not existing
}

bool File_Descriptor_Table::Is_Valid(std::uint16_t id) {
	int count = this->descriptors.size();
	if (id >= count) {
		return false;
	}

	if (this->descriptors[id] == nullptr) {
		return false;
	}

	return true;
}