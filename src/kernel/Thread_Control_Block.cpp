#include "Thread_Control_Block.h"


kiv_os::THandle Thread_Control_Block::get_tid() const {
	return static_cast<kiv_os::THandle>( // cast because inspections are retarded
		std::hash<std::thread::id>()(instance.get_id())
	);
}

kiv_os::THandle Thread_Control_Block::get_ppid() const {
	return ppid;
}

Thread_State Thread_Control_Block::get_state() const {
	return state;
}

bool Thread_Control_Block::is_current() const {
	return std::this_thread::get_id() == instance.get_id();
}
