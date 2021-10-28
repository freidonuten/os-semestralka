#include "Process_Control_Block.h"


Process_Control_Block::Process_Control_Block()
{ }

Process_Control_Block::Process_Control_Block(const kiv_os::THandle pid)
	: pid(pid)
{ }

const kiv_os::THandle Process_Control_Block::get_pid() const
{
	return pid;
}

const Execution_State Process_Control_Block::get_state() const
{
	return state;
}

const char* Process_Control_Block::get_cwd() const
{
	return cwd;
}

const bool Process_Control_Block::is_main_thread(const kiv_os::THandle tid) const {
	return this->tid == tid;
}
