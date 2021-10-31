#include "echo.h"
#include "global.h"

size_t __stdcall echo(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	std::string output = "";
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	char* parameters = reinterpret_cast<char *>(regs.rdi.r);
	size_t written_chars = 0;

	if (strlen(parameters) == 0) {
		output.append(is_echo_on ? ECHO_IS_ON : ECHO_IS_OFF);
		kiv_os_rtl::Write_File(stdout_handle, output.data(), output.size(), written_chars);
	} else {
		if (parameters == AT_SIGN.data()) {
			// Pokud je na prvnim miste v parametrech @ preskocime ho
			parameters++;
		}
		else if (strcmp(ECHO_ON.data(), parameters)) {
			is_echo_on = true;
		}
		else if (strcmp(ECHO_OFF.data(), parameters)) {
			is_echo_on = false;
		}
		else {
			output.append(parameters);
			output.append(new_line);
			kiv_os_rtl::Write_File(stdout_handle, output.data(), output.size(), written_chars);
		}
	}
	kiv_os_rtl::Exit(exit_code);
	return 0;
}
