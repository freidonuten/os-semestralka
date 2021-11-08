#include "delete_file_action.h"

#include "../utils/global_constants.h"

void Delete_File_Action::Run() {
	char* path_argument = reinterpret_cast<char*>(this->registers->rdx.r);

	auto root_directory = this->vfs->Get_Root();
	std::shared_ptr<Path> path_to_file = this->vfs->Get_Path(path_argument);
	std::shared_ptr<Path> path_to_parent = path_to_file->Get_Parent();

	std::shared_ptr<VFS_Element> parent_element = path_to_parent->Get_Element(root_directory);
	char filename[MAX_FILENAME_SIZE];
	path_to_file->Read_Filename(filename);
	
	parent_element->Remove_Child(filename);
}