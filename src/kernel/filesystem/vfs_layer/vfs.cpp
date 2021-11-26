#include "vfs.h"
#include "../sysfs_layer/stream.h"

VFS::VFS() {
	std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(1024 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(1024 * 1024, 1, 512, 12);
	this->handler_table = std::make_shared<Handler_Table>();
	this->path_handlers = std::make_shared<Path_Handlers>(handler_table);

	// initialize stdio -- not sure if this is
	// the right place to be doing this... Probably not.
	const auto [console_input, console_output] = stream::Factory();

	// these descriptors need to be 0 and 1
	handler_table->Create_Descriptor(console_input);
	handler_table->Create_Descriptor(console_output);


	this->element_factory = std::make_shared<VFS_Fat_Element_Factory>(disk, info);
	this->root = this->element_factory->Make_Root_Directory();
	this->root->Create();
	std::shared_ptr<Path> temp_cwd = std::make_shared<Path>("");
	this->cwd_holder = std::make_unique<Dummy_CWD_Holder>(temp_cwd, this->root);
	this->cwd_opener = std::make_unique<Path_Dir_Opener>(this->root, this->element_factory);
}

std::shared_ptr<Handler_Table> VFS::Get_Handler_Table() {
	return this->handler_table;
}

std::shared_ptr<Path_Handlers> VFS::Get_Path_Handlers() {
	return this->path_handlers;
}

std::tuple<std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> VFS::Get_CWD() {
	return this->cwd_holder->Get_Value();
}

void VFS::Set_CWD(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory> directory) {
	this->cwd_holder->Set_Value(cwd, directory);
}

std::shared_ptr<VFS_Directory> VFS::Get_Root() {
	return this->root;
}

std::tuple<std::shared_ptr<VFS_Directory>, Open_Directory_Error> VFS::Open_Directory(std::shared_ptr<Path> cwd) {
	return this->cwd_opener->Open_Directory(cwd);
}

std::shared_ptr<VFS_Fat_Element> VFS::Make_File(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes) {
	return this->element_factory->Make(parent_directory, file_name, file_attributes);
}