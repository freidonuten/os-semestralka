#pragma once

#include "../vfs2/vfs_directory.h"
#include "cwd.h"
#include "../utils/global_constants.h"
#include "../vfs2/vfs_fat_element_factory.h"

#include <tuple>
#include <memory>

class CWD_Opener {
private:
	std::shared_ptr<VFS_Directory2> root;
	std::shared_ptr<VFS_Fat_Element_Factory2> factory;
public:
	CWD_Opener(std::shared_ptr<VFS_Directory2> root, std::shared_ptr<VFS_Fat_Element_Factory2> factory);
	std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error> Open_Directory(std::shared_ptr<CWD> cwd);
};
