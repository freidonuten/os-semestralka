#pragma once

#include <thread>
#include "../api/api.h"
#include "Process_Control_Block.h"
#include "State.h"


class Thread_Control_Block final {
private:
	kiv_os::THandle ppid; // parent process id
	uint32_t return_code;
	kiv_hal::TRegisters context;
	std::thread instance;
	Execution_State state = Execution_State::FREE;
	char** args; // null terminated strings?


public:
	static constexpr kiv_os::THandle get_tid_of(const std::thread::id system_id);

	kiv_os::THandle get_tid() const;
	kiv_os::THandle get_ppid() const;
	Execution_State get_state() const;
	bool is_current() const;

	// TODO: state manipulation
	void allocate(const kiv_os::TThread_Proc& entry, const kiv_hal::TRegisters& regs);
	void adopt(Process_Control_Block& parent);
};
