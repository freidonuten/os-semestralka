#include "echo.h"
#include "global.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	char* parameters = reinterpret_cast<char *>(regs.rdi.r);

	size_t chars_written = 0;
	const auto buffer = std::string_view(parameters);

	kiv_os_rtl::Write_File(stdout_handle, buffer.data(), buffer.size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
