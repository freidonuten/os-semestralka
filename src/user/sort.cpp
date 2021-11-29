#include "sort.h"
#include "utils.h"

size_t __stdcall sort(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	kiv_os::THandle stdin_handle = regs.rax.x;
	kiv_os::THandle file_handle;
	const char* p_filename = reinterpret_cast<const char*>(regs.rdi.r);
	size_t chars_written = 0;
	std::string file_content = "";
	kiv_os::NOS_Error error;


	if (strlen(p_filename) == 0) {
		file_handle = stdin_handle;
	}
	else {
		const auto error = kiv_os_rtl::Open_File(
			p_filename, utils::get_file_attrs(), kiv_os::NOpen_File::fmOpen_Always, file_handle
		);

		if (error != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(error);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return 2;
		}
	}

	char buffer[BUFFER_SIZE];
	for (size_t chars_read = 1; chars_read;) {
		kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		if (chars_read > 0) {
			file_content.append(buffer, chars_read);
		}
	}

	error =kiv_os_rtl::Close_Handle(file_handle);
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	std::vector<std::string> file_lines;
	std::istringstream iss(file_content);
	std::string line;

	while (std::getline(iss, line, '\n')) {
		file_lines.push_back(line);
	}

	file_content.clear();
	std::sort(file_lines.begin(), file_lines.end());
	std::for_each(file_lines.begin(), file_lines.end(), [&file_content](const auto &line) {
		file_content.append(line + '\n');
	});

	kiv_os_rtl::Write_File(stdout_handle, file_content.data(), file_content.size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
