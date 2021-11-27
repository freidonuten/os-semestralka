#include "handler_table.h"

std::uint16_t Handler_Table::Create_Descriptor(std::shared_ptr<VFS_Element> element) {
	std::uint16_t count = static_cast<std::uint16_t>(this->descriptors.size());
	for (std::uint16_t i = 0; i < count; i++) {
		if (this->descriptors[i] == nullptr) {
			this->descriptors[i] = element;
			return i;
		}
	}

	this->descriptors.push_back(element);

	return count;
}

std::shared_ptr<VFS_Element> Handler_Table::Get_Element(std::uint16_t id) {
	return Is_Valid(id)
		? this->descriptors[id]
		: nullptr;
}

std::shared_ptr<VFS_Element> Handler_Table::Remove_Element(std::uint16_t id) {
	if (Is_Valid(id)) {
		auto result = this->descriptors[id];
		this->descriptors[id] = nullptr;
		return result;
	}

	return  nullptr;
}

bool Handler_Table::Is_Valid(std::uint16_t id) {
	return id < descriptors.size() && descriptors[id];
}