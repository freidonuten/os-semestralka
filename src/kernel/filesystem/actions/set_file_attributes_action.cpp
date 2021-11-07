#include "set_file_attributes_action.h"

void Set_File_Attributes_Action::Run() {
	char* path_argument = reinterpret_cast<char*>(this->registers->rdx.r);
	std::uint16_t file_attributes = this->registers->rdi.r;
	auto root_directory = this->vfs->Get_Root();

	std::shared_ptr<Path> path_to_file = this->vfs->Get_Path(path_argument);
	std::shared_ptr<VFS_Element> element = path_to_file->Get_Element(root_directory);

	element->Change_Attributes(file_attributes);
}