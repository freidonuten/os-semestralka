#include "tasklist.h"
#include "utils.h"
#include <array>

size_t __stdcall tasklist(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto [handle, error] = kiv_os_rtl::Open_File(PROC_FILE, 0);
	auto buffer = std::array<char, BUFFER_SIZE>();

	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(1);
	}

	auto processes = std::string(TASKLIST_TITLE);

	for (size_t count = 1; count; ) {
		std::tie(count, std::ignore) = kiv_os_rtl::Read_File(handle, buffer);
		processes.append(buffer.data(), count);
	}

	kiv_os_rtl::Write_File(stdout_handle, processes);
	KIV_OS_EXIT(0);
}
