#include "rd.h"
#include "utils.h"

size_t __stdcall rd(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	char* p_filename = reinterpret_cast<char *>(regs.rdi.r);
	size_t chars_written = 0;
	kiv_os::NOS_Error error;

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::string filename(p_filename);
	error = kiv_os_rtl::Delete_File(filename);

	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	kiv_os_rtl::Exit(0);
	return 0;
}
