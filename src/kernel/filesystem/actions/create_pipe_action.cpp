#include "create_pipe_action.h"
#include "../sysfs_layer/pipe.h"

void Create_Pipe_Action::Run() {
	auto [write_end, read_end] = Pipe::Factory();

	auto desc_table = vfs->Get_Descriptor_Table();
	auto ptr = reinterpret_cast<kiv_os::THandle*>(this->registers->rdx.r);

	// output is write end and read end of the pipe
	ptr[0] = desc_table->Create_Descriptor(write_end);
	ptr[1] = desc_table->Create_Descriptor(read_end);
}