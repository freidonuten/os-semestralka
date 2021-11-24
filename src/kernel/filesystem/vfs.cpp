#include "vfs.h"
#include "sysfs_layer/stream.h"

VFS::VFS() {
	std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(1024 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(1024 * 1024, 1, 512, 12);
	this->descriptor_table = std::make_shared<File_Descriptor_Table>();

	// initialize stdio -- not sure if this is
	// the right place to be doing this... Probably not.
	const auto [console_input, console_output] = stream::Factory();

	// these descriptors need to be 0 and 1
	descriptor_table->Create_Descriptor(console_input);
	descriptor_table->Create_Descriptor(console_output);


	this->element_factory2 = std::make_shared<VFS_Fat_Element_Factory>(disk, info);
	this->root2 = this->element_factory2->Make_Root_Directory();
	std::shared_ptr<CWD> temp_cwd = std::make_shared<CWD>("");
	this->cwd_holder = std::make_unique<Dummy_CWD_Holder>(temp_cwd, this->root2);
	this->cwd_opener = std::make_unique<CWD_Opener>(this->root2, this->element_factory2);
}

std::shared_ptr<File_Descriptor_Table> VFS::Get_Descriptor_Table() {
	return this->descriptor_table;
}

std::tuple<std::shared_ptr<CWD>, std::shared_ptr<VFS_Directory>> VFS::Get_CWD() {
	return this->cwd_holder->Get_Value();
}

void VFS::Set_CWD(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory> directory) {
	this->cwd_holder->Set_Value(cwd, directory);
}

std::shared_ptr<VFS_Directory> VFS::Get_Root2() {
	return this->root2;
}

std::tuple<std::shared_ptr<VFS_Directory>, Open_Directory_Error> VFS::Open_Directory(std::shared_ptr<CWD> cwd) {
	return this->cwd_opener->Open_Directory(cwd);
}

std::shared_ptr<VFS_Fat_Element> VFS::Make_File(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes) {
	return this->Make_File(parent_directory, file_name, file_attributes);
}