#pragma once

#include <unordered_set>
#include <unordered_map>
#include <array>
#include "State.h"
#include "../api/hal.h"
#include "Thread_Control_Block.h"
#include "filesystem/cwd/dummy_cwd_holder.h"


class Process_Control_Block final {
private:
	using Thread_Pool = std::unordered_map<kiv_os::THandle, Thread_Control_Block>;

	kiv_os::THandle pid = 0;
	kiv_os::THandle tid = 0;
	Execution_State state = Execution_State::FREE;
	Thread_Pool thread_list{};
	std::string name = "";
	std::string args = "";

public:
	Process_Control_Block() = default;
	Process_Control_Block(const kiv_os::THandle pid);
	Process_Control_Block(const kiv_os::THandle pid, const kiv_os::THandle tid);

	kiv_os::THandle get_pid() const;
	kiv_os::THandle get_tid() const;
	Execution_State get_state() const;
	bool is_main_thread(const kiv_os::THandle tid) const;
	const char* get_args() const;
	const char* get_name() const;
	
	kiv_os::THandle thread_insert(const kiv_os::TThread_Proc entry_point, const kiv_hal::TRegisters& context);
	void thread_remove(const kiv_os::THandle tid);
	void allocate();
	void exit(const uint16_t code);
	void signal(const kiv_os::NSignal_Id signal);
	void terminate();
	void set_args(const char* args);
	void set_name(const char* name);

	Thread_Control_Block& get_thread(const kiv_os::THandle handle);
};
