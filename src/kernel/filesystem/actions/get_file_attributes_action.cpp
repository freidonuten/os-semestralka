#include "get_file_attributes_action.h"

void Get_File_Attributes_Action::Run() {
	char* path_attr = reinterpret_cast<char*>(this->registers->rdx.r);
	auto root_directory = this->vfs->Get_Root();

	std::shared_ptr<Path> path_to_file = vfs->Get_Path(path_attr);
	std::shared_ptr<VFS_Element> element = path_to_file->Get_Element(root_directory);

	auto attrs = element->Read_Attributes();
	this->registers->rdi.r = attrs;
}