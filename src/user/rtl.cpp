#include "rtl.h"

std::atomic<kiv_os::NOS_Error> kiv_os_rtl::Last_Error;

/*
	Default signal handler
*/

void kiv_os_rtl::Default_Signal_Handler() {
	return;
}

/*
	Prepare register for system call
*/

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

/*
	FILESYSTEM
*/

bool kiv_os_rtl::Read_File(const kiv_os::THandle file_handle, char* const buffer, const size_t buffer_size, size_t &read) {
	kiv_hal::TRegisters regs =  Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Read_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;	
	
	const bool result = kiv_os::Sys_Call(regs);
	read = regs.rax.r;
	return result;
}

bool kiv_os_rtl::Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Write_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;

	const bool result = kiv_os::Sys_Call(regs);
	written = regs.rax.r;
	return result;
}

bool kiv_os_rtl::Open_File(const std::string& filename, std::uint8_t attributes, kiv_os::NOpen_File flags, kiv_os::THandle &open) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File));
	regs.rdx.r = reinterpret_cast<uint64_t>(filename.data());
	regs.rcx.r = static_cast<uint64_t>(flags);
	regs.rdi.r = static_cast<uint64_t>(attributes);
	const bool exit_code = kiv_os::Sys_Call(regs);
	open = regs.rax.x;
	return exit_code;
}

bool kiv_os_rtl::Seek(kiv_os::THandle handle, kiv_os::NFile_Seek operation, kiv_os::NFile_Seek from_position, size_t &position) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Seek));
	regs.rdx.x = handle;
	regs.rdi.r = position;
	regs.rcx.l = static_cast<uint8_t>(operation);

	if (operation == kiv_os::NFile_Seek::Set_Position || operation == kiv_os::NFile_Seek::Set_Size) {
		regs.rcx.x = static_cast<uint16_t>(from_position);
	}
	
	const bool exit_code = kiv_os::Sys_Call(regs);
	
	if (operation == kiv_os::NFile_Seek::Get_Position) {
		position = regs.rax.x;
	}
	return exit_code;
}

bool kiv_os_rtl::Close_Handle(kiv_os::THandle handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));
	regs.rdx.x = handle;
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

bool kiv_os_rtl::Delete_File(const std::string &filename) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Delete_File));
	regs.rdx.r = reinterpret_cast<uint64_t>(filename.data());
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

bool kiv_os_rtl::Set_Working_Dir(const std::string &path) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Set_Working_Dir));
	regs.rdx.r = reinterpret_cast<uint64_t>(path.data());
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

bool kiv_os_rtl::Get_Working_Dir(const std::string &fileName, const size_t filename_lenght, size_t &chars_written) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Get_Working_Dir));
	regs.rdx.r = reinterpret_cast<uint64_t>(fileName.data());
	regs.rcx.r = static_cast<uint64_t>(filename_lenght);
	const bool exit_code = kiv_os::Sys_Call(regs);
	chars_written = regs.rax.r;
	return exit_code;
}

bool kiv_os_rtl::Create_Pipe(kiv_os::THandle *handles) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Create_Pipe));
	regs.rdx.r = reinterpret_cast<uint64_t>(handles);
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

/*
	PROCESS
*/

bool kiv_os_rtl::Create_Process(const std::string &name, const std::string &arguments, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle & new_process) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));
	regs.rcx.r = static_cast<uint64_t>(kiv_os::NClone::Create_Process);
	regs.rdx.r = reinterpret_cast<uint64_t>(name.data());
	regs.rdi.r = reinterpret_cast<uint64_t>(arguments.data());
	regs.rbx.e = (handle_stdin << 16) | handle_stdout;
	const bool exit_code = kiv_os::Sys_Call(regs);
	new_process = regs.rax.x;
	return exit_code;
}

bool kiv_os_rtl::Create_Thread(void *name, void *data, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle &new_process) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));
	regs.rcx.r = static_cast<uint64_t>(kiv_os::NClone::Create_Thread);
	regs.rdx.r = reinterpret_cast<uint64_t>(name);
	regs.rdi.r = reinterpret_cast<uint64_t>(data);
	regs.rbx.e = 1; // FIXME this is hack
	const bool exit_code = kiv_os::Sys_Call(regs);
	new_process = static_cast<kiv_os::THandle>(regs.rax.r);
	return exit_code;
}

bool kiv_os_rtl::Wait_For(kiv_os::THandle *handles, const size_t count_handles, kiv_os::THandle &index) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));
	regs.rdx.r = reinterpret_cast<uint64_t>(handles);
	regs.rcx.r = static_cast<uint64_t>(count_handles);
	const bool exit_code = kiv_os::Sys_Call(regs);
	index = static_cast<kiv_os::THandle>(regs.rax.r);
	return exit_code;
}

bool kiv_os_rtl::Read_Exit_Code(const kiv_os::THandle handle, uint16_t &readed_exit_code) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	regs.rdx.x = handle;
	const bool exit_code = kiv_os::Sys_Call(regs);
	readed_exit_code = regs.rcx.x;
	return exit_code;
}

bool kiv_os_rtl::Exit(uint16_t exit_process_thread_code) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Exit));
	regs.rcx.x = exit_process_thread_code;
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

bool kiv_os_rtl::Shutdown() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Shutdown));
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}

bool kiv_os_rtl::Register_Signal_Handler(kiv_os::NSignal_Id signalId, kiv_os::TThread_Proc thread_proc) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Register_Signal_Handler));
	regs.rcx.r = static_cast<uint64_t>(signalId);

	if (thread_proc != 0) {
		regs.rdx.r = reinterpret_cast<uint64_t>(thread_proc);
	} else {
		regs.rdx.r = reinterpret_cast<uint64_t>(Default_Signal_Handler);

	}
	const bool exit_code = kiv_os::Sys_Call(regs);
	return exit_code;
}
