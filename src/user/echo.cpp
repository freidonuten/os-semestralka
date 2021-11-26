#include "echo.h"
#include "global.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	char* parameters = reinterpret_cast<char *>(regs.rdi.r);
	size_t chars_written = 0;
	std::ostringstream oss(parameters);
	kiv_os_rtl::Write_File(stdout_handle, oss.str().data(), oss.str().size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
