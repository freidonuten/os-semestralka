#pragma once

#include "../../../api/hal.h"
#include "../fat_layer/disk.h"
#include "../fat_layer/filesystem_info.h"
#include "handler_table.h"
#include "path_handlers.h"

#include "../cwd/dummy_cwd_holder.h"
#include "../cwd/cwd_opener.h"
#include "vfs_fat_element_factory.h"

#include <memory>
#include <tuple>



class VFS final {
private:
	std::shared_ptr<Handler_Table> handler_table;
	std::shared_ptr<Path_Handlers> path_handlers;

	std::shared_ptr<VFS_Fat_Element_Factory> element_factory;
	std::unique_ptr<Dummy_CWD_Holder> cwd_holder;
	std::shared_ptr<VFS_Directory> root;
	std::unique_ptr<Path_Dir_Opener> cwd_opener;
	
public:
	VFS();

	std::shared_ptr<Handler_Table> Get_Handler_Table();
	std::shared_ptr<Path_Handlers> Get_Path_Handlers();

	std::tuple<std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> Get_CWD();
	void Set_CWD(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory> directory);
	std::shared_ptr<VFS_Directory> Get_Root();
	std::tuple<std::shared_ptr<VFS_Directory>, Open_Directory_Error> Open_Directory(std::shared_ptr<Path> cwd);

	std::shared_ptr<VFS_Fat_Element> Make_File(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes);
};