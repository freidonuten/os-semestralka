#pragma once

#include "../../api/hal.h"
#include "fat_layer/disk.h"
#include "fat_layer/filesystem_info.h"
#include "vfs_layer/vfs_element.h"
#include "vfs_layer/path.h"
#include "vfs_layer/file_descriptor.h"

#include "cwd/dummy_cwd_holder.h"
#include "cwd/cwd_opener.h"
#include "vfs2/vfs_fat_element_factory.h"

#include <memory>
#include <tuple>



class VFS final {
private:
	std::shared_ptr<VFS_Element_Factory> element_factory;
	std::shared_ptr<VFS_Element> root;
	std::shared_ptr<Path> current_path;
	std::shared_ptr<File_Descriptor_Table> descriptor_table;


	//na cisto dole
	std::shared_ptr<VFS_Fat_Element_Factory2> element_factory2;
	std::unique_ptr<Dummy_CWD_Holder> cwd_holder;
	std::shared_ptr<VFS_Directory2> root2;
	std::unique_ptr<CWD_Opener> cwd_opener;
	
public:
	VFS();

	std::shared_ptr<File_Descriptor_Table> Get_Descriptor_Table();
	std::shared_ptr<VFS_Element> Get_Root();
	std::shared_ptr<Path> Get_Path(char* path); //absolute or relative
	std::shared_ptr<Path> Get_Current_Path();
	void Set_Current_Path(std::shared_ptr<Path> new_path);

	std::tuple<std::shared_ptr<CWD>, std::shared_ptr<VFS_Directory2>> Get_CWD();
	void Set_CWD(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory2>);
	std::shared_ptr<VFS_Directory2> Get_Root2();
	std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error> Open_Directory(std::shared_ptr<CWD> cwd);
};