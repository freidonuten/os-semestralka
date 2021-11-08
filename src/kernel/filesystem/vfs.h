#pragma once

#include "../../api/hal.h"
#include "fat_layer/disk.h"
#include "fat_layer/filesystem_info.h"
#include "vfs_layer/vfs_element.h"
#include "vfs_layer/path.h"
#include "vfs_layer/file_descriptor.h"
#include <memory>

class Action_Factory;
class Action;

class VFS {
private:
	std::shared_ptr<Action_Factory> action_factory;
	std::shared_ptr<VFS_Element_Factory> element_factory;

	std::shared_ptr<VFS_Element> root;
	std::shared_ptr<Path> current_path;
	
	std::shared_ptr<File_Descriptor_Table> descriptor_table;

	
public:
	VFS();

	void Proceed_Action(kiv_hal::TRegisters &regs);

	std::shared_ptr<File_Descriptor_Table> Get_Descriptor_Table();

	std::shared_ptr<VFS_Element> Get_Root();
	std::shared_ptr<Path> Get_Path(char* path); //absolute or relative
	std::shared_ptr<Path> Get_Current_Path();
	void Set_Current_Path(std::shared_ptr<Path> new_path);


};