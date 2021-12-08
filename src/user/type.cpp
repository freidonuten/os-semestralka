#include "type.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>

size_t __stdcall type(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	char* p_filename = reinterpret_cast<char*>(regs.rdi.r);
	size_t chars_read = init_value;
	size_t chars_written = 0;
	size_t file_offset = 0;
	kiv_os::NOS_Error error;
	

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::string filename(p_filename);
	std::string file_content = "";
	
	auto buffer = std::array<char, BUFFER_SIZE>{};

	error = kiv_os_rtl::Open_File(p_filename, utils::get_file_attrs(), kiv_os::NOpen_File::fmOpen_Always, file_handle);

	// kontrola zda se podarilo otevrit soubor
	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	while (1) {
		const auto [count, eof, error] = rtl::Read_File(file_handle, buffer);
		file_content.append(buffer.data(), count);
		if (eof) {
			break;
		}
	}

	error = kiv_os_rtl::Close_Handle(file_handle);
	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	kiv_os_rtl::Write_File(stdout_handle, file_content.data(), file_content.size(), chars_written);
	KIV_OS_EXIT(0);
}
