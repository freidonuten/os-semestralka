#include "Thread_Control_Block.h"


kiv_os::THandle Thread_Control_Block::get_tid() const {
	return static_cast<kiv_os::THandle>( // cast because inspections are retarded
		std::hash<std::thread::id>()(instance.get_id())
	);
}

kiv_os::THandle Thread_Control_Block::get_ppid() const {
	return ppid;
}

Execution_State Thread_Control_Block::get_state() const {
	return state;
}

bool Thread_Control_Block::is_current() const {
	return std::this_thread::get_id() == instance.get_id();
}

void Thread_Control_Block::allocate(const kiv_os::TThread_Proc& entry, const kiv_hal::TRegisters& regs) {
	context = regs;
	instance = std::thread(entry, context); // this will throw system_error on failure
	state = Execution_State::RUNNING;
}

void Thread_Control_Block::adopt(Process_Control_Block& parent) {
	// FIXME: what if this is called twice?
	ppid = parent.get_pid();
	parent.thread_insert(get_tid());
}
