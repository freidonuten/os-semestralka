#pragma once

#include "../vfs_layer/vfs_directory.h"
#include "cwd.h"
#include "../utils/global_constants.h"
#include "../vfs_layer/vfs_fat_element_factory.h"

#include <tuple>
#include <memory>

class CWD_Opener {
private:
	std::shared_ptr<VFS_Directory> root;
	std::shared_ptr<VFS_Fat_Element_Factory> factory;
public:
	CWD_Opener(std::shared_ptr<VFS_Directory> root, std::shared_ptr<VFS_Fat_Element_Factory> factory);
	std::tuple<std::shared_ptr<VFS_Directory>, Open_Directory_Error> Open_Directory(std::shared_ptr<CWD> cwd);
};
