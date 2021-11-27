#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../vfs_layer/vfs_element.h"
#include "../utils/global_structs.h"
#include "../cwd/cwd.h"

class Handler_Table {
private:
	std::vector<std::shared_ptr<VFS_Element>> descriptors;
	bool Is_Valid(std::uint16_t id);

public:
	std::uint16_t Create_Descriptor(std::shared_ptr<VFS_Element> element);
	std::shared_ptr<VFS_Element> Get_Element(std::uint16_t id);
	std::shared_ptr<VFS_Element> Remove_Element(std::uint16_t id);
};