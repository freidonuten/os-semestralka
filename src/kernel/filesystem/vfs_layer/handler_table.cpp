#include "handler_table.h"

std::uint16_t Handler_Table::Create_Descriptor(std::shared_ptr<VFS_Element> element) {
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

std::tuple<std::shared_ptr<VFS_Element>, bool> Handler_Table::Get_Element(std::uint16_t id) {
	if (Is_Valid(id)) {
		return { this->descriptors[id], true };
	}

	return { nullptr, false };
}

std::tuple<std::shared_ptr<VFS_Element>, bool> Handler_Table::Remove_Element(std::uint16_t id) {
	if (Is_Valid(id)) {
		auto result = this->descriptors[id];
		this->descriptors[id] = nullptr;
		return { result, true };
	}
	return  { nullptr, false };
}

bool Handler_Table::Is_Valid(std::uint16_t id) {
	int count = this->descriptors.size();
	if (id >= count) {
		return false;
	}

	if (this->descriptors[id] == nullptr) {
		return false;
	}

	return true;
}