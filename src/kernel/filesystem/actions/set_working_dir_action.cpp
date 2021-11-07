#include "set_working_dir_action.h"

void Set_Working_Dir_Action::Run() {
	char* path_argument = reinterpret_cast<char*>(this->registers->rdx.r);

	auto root_directory = this->vfs->Get_Root();
	std::shared_ptr<Path> new_path = this->vfs->Get_Path(path_argument);
	this->vfs->Set_Current_Path(new_path);
}