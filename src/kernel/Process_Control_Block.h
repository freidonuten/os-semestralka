#pragma once

#include <unordered_set>
#include <unordered_map>
#include "State.h"
#include "../api/hal.h"
#include "Thread_Control_Block.h"


class Process_Control_Block final {
private:
	using Thread_Pool = std::unordered_map<kiv_os::THandle, Thread_Control_Block>;
	using FD_Pool = std::unordered_set<kiv_os::THandle>;
	using Buffer = char[64]; // get rid of the magic number

	kiv_os::THandle pid;
	kiv_os::THandle tid;
	Execution_State state = Execution_State::FREE;
	Thread_Pool thread_list;
	FD_Pool fd_list;
	Buffer cwd = "/";
	

public:
	Process_Control_Block() = default;
	Process_Control_Block(const kiv_os::THandle pid);

	const kiv_os::THandle get_pid() const;
	const kiv_os::THandle get_tid() const;
	const Execution_State get_state() const;
	const char* get_cwd() const;
	const bool is_main_thread(const kiv_os::THandle tid) const;
	
	kiv_os::THandle thread_insert(const kiv_os::TThread_Proc entry_point, const kiv_hal::TRegisters& context);
	void thread_remove(const kiv_os::THandle tid);
	void fd_insert(const kiv_os::THandle fd);
	void fd_remove(const kiv_os::THandle fd);
	void set_cwd(const char* buffer);
	void allocate();
	void exit(const uint16_t code);
	void signal(const kiv_os::NSignal_Id signal);
	void terminate();

	Thread_Control_Block& get_thread(const kiv_os::THandle handle);
};
