#pragma once

#include <unordered_set>
#include "State.h"


class Process_Control_Block final {
private:
	using Thread_Pool = std::unordered_set<kiv_os::THandle>;
	using FD_Pool = std::unordered_set<kiv_os::THandle>;
	using Buffer = char[64]; // get rid of the magic number

	uint32_t pid;
	uint32_t tid;
	Execution_State state = Execution_State::FREE;
	Thread_Pool thread_list;
	FD_Pool fd_list;
	Buffer cwd = "/";
	

public:
	explicit Process_Control_Block();
	explicit Process_Control_Block(const kiv_os::THandle pid);

	const kiv_os::THandle get_pid() const;
	const Execution_State get_state() const;
	const char* get_cwd() const;
	
	void thread_insert(const kiv_os::THandle tid);
	void thread_remove(const kiv_os::THandle tid);
	void fd_insert(const kiv_os::THandle fd);
	void fd_remove(const kiv_os::THandle fd);
	void set_cwd(const char* buffer);
	void allocate();
};
