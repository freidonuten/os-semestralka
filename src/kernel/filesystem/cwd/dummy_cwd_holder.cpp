#include "dummy_cwd_holder.h"

Dummy_CWD_Holder::Dummy_CWD_Holder(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory2> directory) {
	Set_Value(cwd, directory);
}

void Dummy_CWD_Holder::Set_Value(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory2> directory) {
	this->value = { cwd, directory };
}

std::tuple<std::shared_ptr<CWD>, std::shared_ptr<VFS_Directory2>> Dummy_CWD_Holder::Get_Value() {
	return this->value;
}