#include "dummy_cwd_holder.h"

Dummy_CWD_Holder::Dummy_CWD_Holder(std::tuple<CWD*, VFS_Directory2*> value) {
	Set_Value(value);
}

void Dummy_CWD_Holder::Set_Value(std::tuple<CWD*, VFS_Directory2*> value) {
	this->value = value;
}

std::tuple<CWD*, VFS_Directory2*> Dummy_CWD_Holder::Get_Value() {
	return this->value;
}