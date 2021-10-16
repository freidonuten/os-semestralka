#pragma once

class Thread_Control_Block final {
private:
	const kiv_os::THandle TID;	// thread id
	const kiv_os::THandle PPID; // parent process id

	char state; // TODO: make it enum

public:
	// thread always needs a parent
	explicit Thread_Control_Block() = delete;
	explicit Thread_Control_Block(const kiv_os::THandle ppid);

	// TODO: state manipulation
	
};
