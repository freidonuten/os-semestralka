#include "vfs.h"

VFS::VFS() {
	std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(1024 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(1024 * 1024, 1, 512, 12);
	this->element_factory = std::make_shared<VFS_Element_Factory>(disk, info);
	this->root = this->element_factory->Create_Root_Directory();
	char root_name[MAX_FILENAME_SIZE] = "";
	this->current_path = std::make_shared<Path>(root_name);
	this->descriptor_table = std::make_shared<File_Descriptor_Table>();
}

std::shared_ptr<File_Descriptor_Table> VFS::Get_Descriptor_Table() {
	return this->descriptor_table;
}

std::shared_ptr<Path> VFS::Get_Path(char *path) {
	if (path[0] == '/') {
		return std::make_shared<Path>(path + 1);
	}
	else {
		std::shared_ptr<Path> path_to_add = std::make_shared<Path>(path);
		return this->current_path->Append(path_to_add);
	}
}

std::shared_ptr<VFS_Element> VFS::Get_Root() {
	return this->root;
}

std::shared_ptr<Path> VFS::Get_Current_Path() {
	return this->current_path;
}

void VFS::Set_Current_Path(std::shared_ptr<Path> new_path) {
	this->current_path = new_path;
}