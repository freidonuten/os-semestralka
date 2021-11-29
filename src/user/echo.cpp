#include "echo.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	char* parameters = reinterpret_cast<char *>(regs.rdi.r);
	kiv_os::NOS_Error error;
	size_t chars_written = 0;
	const auto buffer = std::string_view(parameters);

	error = kiv_os_rtl::Write_File(stdout_handle, buffer.data(), buffer.size(), chars_written);
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}
	kiv_os_rtl::Write_File(stdout_handle, new_line.data(), new_line.size(), chars_written);

	kiv_os_rtl::Exit(0);
	return 0;
}
