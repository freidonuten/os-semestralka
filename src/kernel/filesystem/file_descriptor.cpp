#include "file_descriptor.h"

File_Descriptor_Table::File_Descriptor_Table() {
	std::vector<std::shared_ptr<File_Descriptor>> temp;
	this->descriptors = temp;
}

std::shared_ptr<File_Descriptor> File_Descriptor_Table::Create_Descriptor(std::uint16_t fat_first_cluster) {
	std::shared_ptr<File_Descriptor> result = std::make_shared<File_Descriptor>();
	result->fat_first_cluster = fat_first_cluster;
	result->position = 0;

	Add_Descriptor(result);
	return result;
}

void File_Descriptor_Table::Add_Descriptor(std::shared_ptr<File_Descriptor> descriptor) {
	int count = this->descriptors.size();

	for (std::uint16_t i = 0; i < count; i++) {
		if (this->descriptors[i] == nullptr) {
			descriptor->descriptor_id = i;
			this->descriptors[i] = descriptor;
			return;
		}
	}

	descriptor->descriptor_id = count;
	this->descriptors.push_back(descriptor);
}

std::shared_ptr<File_Descriptor> File_Descriptor_Table::Get_Descriptor(std::uint16_t id) {

	return this->descriptors[id];
	//TODO Error Handler descriptor not existing
}

void File_Descriptor_Table::Remove_Handle(std::uint16_t id) {
	this->descriptors[id] = nullptr;
	//TODO Error Handler descriptor not existing
}