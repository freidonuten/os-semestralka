#include "close_handle_action.h"

void Close_Handle_Action::Run() {
	auto to_close = this->registers->rdx.x;
	auto descriptor_table = this->vfs->Get_Descriptor_Table();
	descriptor_table->Remove_Descriptor(to_close);
}