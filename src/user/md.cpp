#include "md.h"
#include "utils.h"

size_t __stdcall md(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	char* p_filename = reinterpret_cast<char*>(regs.rdi.r);
	size_t chars_written = 0;
	kiv_os::NOS_Error error;

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::string filename(p_filename);
	kiv_os::NOpen_File flag = static_cast<kiv_os::NOpen_File>(0);
	kiv_os::THandle directory_handle;

	error = kiv_os_rtl::Open_File(filename, utils::get_dir_attrs(), flag, directory_handle);

	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	kiv_os_rtl::Close_Handle(directory_handle);
	kiv_os_rtl::Exit(0);
	return 0;
}