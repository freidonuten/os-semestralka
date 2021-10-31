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

Thread_Control_Block::Thread_Control_Block(
	Process_Control_Block& parent,
	const kiv_os::TThread_Proc entry,
	const kiv_hal::TRegisters& state
)	: parent(parent)
	, native_handle(CreateThread(NULL, NULL, procedure_wrapper, new procedure_args{ entry, state }, NULL, &native_id))
{ }



kiv_os::THandle Thread_Control_Block::current_tid() {
	return static_cast<kiv_os::THandle>(GetCurrentThreadId());
}

kiv_os::THandle Thread_Control_Block::get_tid() const {
	return tid;
}

kiv_os::THandle Thread_Control_Block::get_ppid() const {
	return parent.get_pid();
}

void* Thread_Control_Block::get_native_handle() const {
	return native_handle;
}

bool Thread_Control_Block::is_current() const {
	return GetCurrentThreadId() == native_id;
}

void Thread_Control_Block::register_signal_handle(const kiv_os::NSignal_Id signal, const kiv_os::TThread_Proc handler) {
	signal_handlers.emplace(signal, handler);
}

void Thread_Control_Block::remove_signal_handle(const kiv_os::NSignal_Id signal) {
	signal_handlers.erase(signal);
}

void Thread_Control_Block::exit(const uint16_t exit_code) {
	ExitThread(this->exit_code = exit_code);
}

uint16_t Thread_Control_Block::read_exit_code() {
	exit_code = static_cast<decltype(exit_code)>(WaitForSingleObject(native_handle, INFINITE));

	parent.thread_remove(get_tid());

	return exit_code;
}
