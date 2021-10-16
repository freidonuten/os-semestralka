#pragma once

class Process_Control_Block final {
private:
	using Thread_Pool = std::array<kiv_os::THandle, constants::thread_per_process>;
	using FD_Pool = std::array<kiv_os::THandle, constants::fd_per_process>;

	const kiv_os::THandle PID;
	Thread_Pool thread_pool;
	FD_Pool fd_pool;

public:
	explicit Process_Control_Block() = delete;
	explicit Process_Control_Block(const kiv_os::THandle pid);

	const kiv_os::THandle get_pid() const;
	
	void thread_insert(const kiv_os::THandle tid);
	void thread_remove(const kiv_os::THandle tid);
	void fd_insert(const kiv_os::THandle fd);
	void fd_remove(const kiv_os::THandle fd);
};
