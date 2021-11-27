#include <functional>
#include "Thread_Control_Block.h"
#include "Process_Control_Block.h"


struct procedure_args {
	const kiv_os::TThread_Proc procedure;
	const kiv_hal::TRegisters context;
};

DWORD WINAPI procedure_wrapper(LPVOID ptr) {
	const auto [ call, args ] = *reinterpret_cast<procedure_args*>(ptr);
	delete ptr; // no idea how to push smart_ptr through void* without triggering gc

	call(args);

	return 0;
}

Thread_Control_Block::Thread_Control_Block(kiv_os::THandle ppid)
: ppid(ppid), native_handle(NULL) 
{ }

Thread_Control_Block::Thread_Control_Block(
	kiv_os::THandle ppid,
	const kiv_os::TThread_Proc entry,
	const kiv_hal::TRegisters& state
)	: ppid(ppid)
{
	native_handle = CreateThread(
		NULL,
		NULL,
		procedure_wrapper,
		new procedure_args{ entry, state },
		NULL,
		&native_id
	);
}


kiv_os::THandle Thread_Control_Block::current_tid() {
	return static_cast<kiv_os::THandle>(GetCurrentThreadId());
}

kiv_os::THandle Thread_Control_Block::get_tid() const {
	return tid;
}

kiv_os::THandle Thread_Control_Block::get_ppid() const {
	return ppid;
}

void* Thread_Control_Block::get_native_handle() const {
	return native_handle;
}

bool Thread_Control_Block::is_current() const {
	return GetCurrentThreadId() == native_id;
}

void Thread_Control_Block::register_signal_handle(const kiv_os::TThread_Proc handler) {
	signal_handler = handler;
}

void Thread_Control_Block::signal(const kiv_os::NSignal_Id signal) {
	if (signal_handler) {
		auto regs = kiv_hal::TRegisters{};
		regs.rcx.r = static_cast<uint64_t>(signal);
		signal_handler(regs);
	}
}

void Thread_Control_Block::exit(const uint16_t exit_code) {
	ExitThread(exit_code);
}

uint16_t Thread_Control_Block::read_exit_code() {
	const auto exit_code = static_cast<uint16_t>(WaitForSingleObject(native_handle, INFINITE));

	//parent->thread_remove(get_tid());

	return exit_code;
}
