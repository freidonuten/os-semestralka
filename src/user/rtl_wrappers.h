#pragma once

#include "rtl.h"

#define KIV_OS_EXIT(code) kiv_os_rtl::Exit(code); return code;

// This is a thin wrapper library above our original RTL
// The idea of this wrapper is to completely abolish output params and use
// return values instead. The result is much cleaner and mode C++-sy client code.
namespace rtl {
	template<typename SizedBuffer>
	std::pair<kiv_os::THandle, kiv_os::NOS_Error> Open_File(const SizedBuffer& filename, std::uint8_t attributes, kiv_os::NOpen_File flags = kiv_os::NOpen_File::fmOpen_Always) {
		auto handle = kiv_os::THandle(0);
		return { handle, kiv_os_rtl::Open_File(filename.data(), attributes, flags, handle) };
	}

	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Read_File(const kiv_os::THandle file_handle, SizedBuffer& buf) {
		auto count = size_t(0);
		return { count, kiv_os_rtl::Read_File(file_handle, buf.data(), buf.size(), count) };
	}
	
	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Write_File(const kiv_os::THandle file_handle, SizedBuffer& buf) {
		auto count = size_t(0);
		return { count, kiv_os_rtl::Write_File(file_handle, buf.data(), buf.size(), count) };
	}

	kiv_os::NOS_Error Close_Handle(kiv_os::THandle handle);

	template<typename TerminatedBuffer>
	kiv_os::NOS_Error Delete_File(const TerminatedBuffer& filename) {
		return kiv_os_rtl::Delete_File(filename.data());
	}

	template<typename TerminatedBuffer>
	kiv_os::NOS_Error Set_Working_Directory(const TerminatedBuffer& dirname) {
		return kiv_os_rtl::Set_Working_Dir(dirname.data());
	}

	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Get_Working_Dir(SizedBuffer& buffer) {
		auto count = size_t(0);
		return { count, kiv_os_rtl::Get_Working_Dir(buffer.data(), buffer.size(), count) };
	}

	std::tuple<kiv_os::THandle, kiv_os::THandle, kiv_os::NOS_Error> Create_Pipe();

	template<typename TerminatedBuffer>
	std::pair<kiv_os::THandle, kiv_os::NOS_Error> Create_Process(const TerminatedBuffer& name, const TerminatedBuffer& arguments, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout) {
		auto handle = kiv_os::THandle(0);
		return { handle, kiv_os_rtl::Create_Process(name.data(), arguments.data(), handle_stdin, handle_stdout, handle) };
	}

	std::pair<kiv_os::THandle, kiv_os::NOS_Error> Create_Thread(void* program, void* args, const kiv_os::THandle handle_stdin = 0, const kiv_os::THandle handle_stdout = 1);

	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Wait_For(const SizedBuffer& handles) {
		auto index = size_t(0);
		return { index, kiv_os_rtl::Wait_For(handles.data(), handles.count(), index) };
	}

	size_t Read_Exit_Code(const kiv_os::THandle handle);
	void Exit(uint16_t code);
	void Shutdown();
	kiv_os::NOS_Error Register_Signal_Handler(kiv_os::NSignal_Id signal_id, kiv_os::TThread_Proc thread_proc);
}

