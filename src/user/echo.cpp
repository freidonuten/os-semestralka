#include "echo.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto string = std::string_view(reinterpret_cast<char*>(regs.rdi.r));
	const auto [count, error] = kiv_os_rtl::Write_File(stdout_handle, string);

	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	kiv_os_rtl::Write_File(stdout_handle, new_line);
	KIV_OS_EXIT(0);
}
