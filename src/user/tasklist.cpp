#include "tasklist.h"

size_t __stdcall tasklist(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	std::stringstream processes;
	size_t chars_read = 1;
	size_t chars_written = 0;
	size_t offset = 0;
	char buffer[BUFFER_SIZE];

	kiv_os_rtl::Open_File("procs", kiv_os::NFile_Attributes::Directory, kiv_os::NOpen_File::fmOpen_Always, file_handle);

	if (file_handle == invalid_file_handle) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	while (chars_read) {
		chars_read = 0;
		memset(buffer, 0, BUFFER_SIZE);
		kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, offset);
		kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		processes << buffer;
		offset += chars_read;
	}
	kiv_os_rtl::Close_Handle(file_handle);
	kiv_os_rtl::Write_File(stdout_handle, processes.str().data(), processes.str().size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
