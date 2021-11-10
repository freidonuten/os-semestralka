#include "create_pipe_action.h"
#include "../sysfs_layer/pipe.h"

void Create_Pipe_Action::Run() {
	auto write_end = std::make_shared<Pipe>();
	auto read_end =  std::shared_ptr<Pipe>(write_end);
	auto desc_table = vfs->Get_Descriptor_Table();
	auto ptr = reinterpret_cast<kiv_os::THandle*>(this->registers->rdx.r);

	// output is write end and read end of the pipe
	ptr[0] = desc_table->Create_Descriptor(std::static_pointer_cast<VFS_Element>(write_end));
	ptr[1] = desc_table->Create_Descriptor(std::static_pointer_cast<VFS_Element>(read_end));
}