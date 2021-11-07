#include "get_working_dir_action.h"

void Get_Working_Dir_Action::Run() {
	char* buffer = reinterpret_cast<char*>(this->registers->rdx.r);
	std::uint64_t buffer_size = this->registers->rcx.r;

	auto current_path = this->vfs->Get_Current_Path();
	std::uint64_t read_chars = current_path->Read_Path(buffer, buffer_size);

	this->registers->rdi.r = read_chars;
}