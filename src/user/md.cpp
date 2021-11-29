#include "md.h"
#include "utils.h"

size_t __stdcall md(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto filename = std::string_view(reinterpret_cast<char*>(regs.rdi.r));

	if (!filename.size()) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT);
		KIV_OS_EXIT(1);
	}

	const auto flag = static_cast<kiv_os::NOpen_File>(0);
	const auto [ dir_handle, error ] = kiv_os_rtl::Open_File(filename, utils::get_dir_attrs(), flag);

	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	const auto error2 = kiv_os_rtl::Close_Handle(dir_handle);
	if (error2 != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}
	
	KIV_OS_EXIT(0);
}