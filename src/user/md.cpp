#include "md.h"
#include "utils.h"
#include "rtl_wrappers.h"

size_t __stdcall md(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto filename = std::string_view(reinterpret_cast<char*>(regs.rdi.r));

	// Check lenght of filename
	if (!filename.size()) {
		rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT);
		KIV_OS_EXIT(1);
	}

	// File flag
	const auto flag = static_cast<kiv_os::NOpen_File>(0);
	// Open file
	const auto [ dir_handle, error ] = rtl::Open_File(filename, utils::get_dir_attrs(), flag);

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	// Close file handle
	const auto error2 = kiv_os_rtl::Close_Handle(dir_handle);
	if (error2 != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}
	
	KIV_OS_EXIT(0);
}