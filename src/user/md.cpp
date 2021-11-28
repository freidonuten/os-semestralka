#include "md.h"

size_t __stdcall md(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	char* p_filename = reinterpret_cast<char*>(regs.rdi.r);
	size_t chars_written = 0;

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::string filename(p_filename);
	kiv_os::NOpen_File flag = static_cast<kiv_os::NOpen_File>(0);
	kiv_os::THandle directory_handle;

	std::uint8_t file_attrs = static_cast<std::uint8_t>(kiv_os::NFile_Attributes::Directory) + static_cast<std::uint8_t>(kiv_os::NFile_Attributes::Read_Only);
	kiv_os_rtl::Open_File(filename, file_attrs, flag, directory_handle);

	if (directory_handle == invalid_file_handle) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	kiv_os_rtl::Close_Handle(directory_handle);
	kiv_os_rtl::Exit(0);
	return 0;
}