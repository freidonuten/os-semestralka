#include "rd.h"
#include "utils.h"
#include "rtl_wrappers.h"

size_t __stdcall rd(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto filename = std::string(reinterpret_cast<char*>(regs.rdi.r));

	// Check filename lenght
	if (!filename.size()) {
		rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND);
		KIV_OS_EXIT(1);
	}

	// Delete file
	const auto error = rtl::Delete_File(filename);

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	KIV_OS_EXIT(0);
}
