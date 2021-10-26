#include "Process_Control_Block.h"


const bool Process_Control_Block::is_main_thread(const kiv_os::THandle tid) const {
	return this->tid == tid;
}
