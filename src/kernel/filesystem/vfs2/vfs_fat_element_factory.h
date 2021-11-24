#pragma once

#include "vfs_fat_element.h"
#include "vfs_directory.h"
#include "vfs_file.h"

#include "../fat_layer/fat_directory.h"
#include "../fat_layer/fat_file.h"

#include <cstdint>
#include <memory>

#include "../utils/global_structs.h"
#include "../utils/api_utils.h"
#include "../../../api/api.h"

class VFS_Fat_Element_Factory {
private:
	std::shared_ptr<Fat_File_Factory> file_factory;
	std::shared_ptr<Fat_Directory_Factory> directory_factory;

	VFS_Fat_Element_Type Get_Element_Type(std::uint8_t file_attributes);

public:
	VFS_Fat_Element_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::shared_ptr<VFS_Fat_Element> Make(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes);
	std::shared_ptr<VFS_Directory> Make_Root_Directory();
};