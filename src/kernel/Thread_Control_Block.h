#pragma once

#include <thread>
#include "../api/api.h"
#include "Process_Control_Block.h"


enum class Thread_State {
	FREE = 0,
	BLOCKED,
	READY,
	RUNNING
};

class Thread_Control_Block final {
private:
	kiv_os::THandle ppid; // parent process id
	uint32_t return_code;
	kiv_hal::TRegisters context;
	std::thread instance;
	Thread_State state = Thread_State::FREE;
	char** args; // null terminated strings?


public:

	kiv_os::THandle get_tid() const;
	kiv_os::THandle get_ppid() const;
	Thread_State get_state() const;
	bool is_current() const;

	// TODO: state manipulation
	void allocate(const kiv_os::TThread_Proc& entry, const kiv_hal::TRegisters& context);
	void adopt(Process_Control_Block& parent);
};
