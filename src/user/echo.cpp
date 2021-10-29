#include "echo.h"
#include "global.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdoutHandle = regs.rbx.x;
	std::string output = "";
	uint16_t exitCode = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	char* parameters = reinterpret_cast<char *>(regs.rdi.r);
	size_t writtenChars = 0;

	if (strlen(parameters) == 0) {
		isEchoOn ? output.append(ECHO_IS_ON) : output.append(ECHO_IS_OFF);
		kiv_os_rtl::Write_File(stdoutHandle, output.data(), output.size(), writtenChars);
	} else {
		if (parameters == AT_SIGN.data()) {
			// Pokud je na prvnim miste v parametrech @ preskocime ho
			parameters++;
		}
		else if (strcmp(ECHO_ON.data(), parameters)) {
			isEchoOn = true;
		}
		else if (strcmp(ECHO_OFF.data(), parameters)) {
			isEchoOn = false;
		}
		else {
			output.append(parameters);
			output.append(newLine);
			kiv_os_rtl::Write_File(stdoutHandle, output.data(), output.size(), writtenChars);
		}
	}

	kiv_os_rtl::Exit(exitCode);
	return 0;
}
