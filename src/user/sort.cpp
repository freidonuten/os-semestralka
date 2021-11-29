#include "sort.h"
#include "utils.h"

size_t __stdcall sort(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle stdout_handle = regs.rbx.x;
	kiv_os::THandle file_handle;
	const char* p_filename = reinterpret_cast<const char*>(regs.rdi.r);
	size_t chars_written = 0;
	size_t chars_read = 1;
	size_t offset = 0;
	std::string file_content = "";
	std::vector<std::string> file_lines;
	char buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	const std::string filename(p_filename);
	kiv_os_rtl::Open_File(filename, utils::get_file_attrs(), kiv_os::NOpen_File::fmOpen_Always, file_handle);

	if (file_handle == invalid_file_handle) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	while (chars_read) {
		chars_read = 0;
		kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, offset);
		kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		if (chars_read > 0) {
			file_content.append(buffer, chars_read);
		}
		offset += chars_read;
	}

	kiv_os_rtl::Close_Handle(file_handle);

	std::istringstream iss(file_content);
	std::string line;
	while (std::getline(iss, line, '\n')) {
		file_lines.push_back(line);
	}

	file_content.clear();
	std::sort(file_lines.begin(), file_lines.end());
	std::for_each(file_lines.begin(), file_lines.end(), [&file_content](const auto &line) {
		file_content.append(line);
	});

	kiv_os_rtl::Write_File(stdout_handle, file_content.data(), file_content.size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
