#include "tasklist.h"
#include "utils.h"

size_t __stdcall tasklist(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	std::stringstream processes;
	size_t chars_read = 1;
	size_t chars_written = 0;
	size_t offset = 0;
	char buffer[BUFFER_SIZE];
	kiv_os::NOS_Error error;

	error = kiv_os_rtl::Open_File("\\proc", 0, kiv_os::NOpen_File::fmOpen_Always, file_handle);

	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	processes << TASKLIST_TITLE;
	while (chars_read) {
		chars_read = 0;
		memset(buffer, 0, BUFFER_SIZE);
		kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		processes << buffer;
		offset += chars_read;
	}
	kiv_os_rtl::Write_File(stdout_handle, processes.str().data(), processes.str().size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
