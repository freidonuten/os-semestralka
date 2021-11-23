#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../vfs2/vfs_element.h"

class File_Descriptor_Table {
private:
	std::vector<std::shared_ptr<VFS_Element2>> descriptors;
	bool Is_Valid(std::uint16_t id);
public:
	std::uint16_t Create_Descriptor(std::shared_ptr<VFS_Element2> element);
	std::shared_ptr<VFS_Element2> Get_Descriptor(std::uint16_t id);
	void Remove_Descriptor(std::uint16_t id);


};