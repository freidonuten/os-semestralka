#include "rtl_wrappers.h"
#include <array>

/*
	This is a thin wrapper library above our original RTL
	The idea of this wrapper is to completely abolish output params and use
	return values instead. The result is much cleaner and mode C++-sy client code.
*/

std::pair<kiv_os::THandle, kiv_os::NOS_Error> rtl::Create_Thread(void* program, void* args, const kiv_os::THandle handle_stdin, const kiv_os::THandle handle_stdout) {
	auto handle = kiv_os::THandle(0);
	return { handle, kiv_os_rtl::Create_Thread(program, args, handle_stdin, handle_stdout, handle) };
}

size_t rtl::Read_Exit_Code(const kiv_os::THandle handle) {
	auto code = uint16_t(0);
	kiv_os_rtl::Read_Exit_Code(handle, code);
	return code;
}

void rtl::Exit(uint16_t code) {
	kiv_os_rtl::Exit(code);
}

void rtl::Shutdown() {
	kiv_os_rtl::Shutdown();
}

kiv_os::NOS_Error rtl::Register_Signal_Handler(kiv_os::NSignal_Id signal_id, kiv_os::TThread_Proc thread_proc) {
	return kiv_os_rtl::Register_Signal_Handler(signal_id, thread_proc);
}

kiv_os::NOS_Error rtl::Close_Handle(kiv_os::THandle handle) {
	return kiv_os_rtl::Close_Handle(handle);
}

std::tuple<kiv_os::THandle, kiv_os::THandle, kiv_os::NOS_Error> rtl::Create_Pipe() {
	auto handles = std::array<kiv_os::THandle, 2>{};
	return { handles[0], handles[1], kiv_os_rtl::Create_Pipe(handles.data()) };
}
