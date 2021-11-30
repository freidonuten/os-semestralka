#include "find.h"
#include "utils.h"
#include <array>

size_t __stdcall find(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	kiv_os::THandle stdin_handle = regs.rax.x;
	kiv_os::THandle stdout_handle = regs.rbx.x;
	const char* parameters = reinterpret_cast<const char*>(regs.rdi.r);
	size_t chars_written = 0;
	size_t line_counter = 0;
	std::array<char, BUFFER_SIZE> buffer = std::array<char, BUFFER_SIZE>();
	std::string line;
	std::string parameter;
	std::string filename;
	std::string word;
	std::string file_content = "";
	bool print_count_lines = false;
	bool print_not_contain_lines = false;
	kiv_os::NOS_Error error;

	if (strlen(parameters) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::stringstream ss(parameters);

	while (ss >> parameter) {
		if (parameter == "/V" || parameter == "/v") {
			print_not_contain_lines = true;
		}
		else if (parameter == "/C" || parameter == "/c") {
			print_count_lines = true;
		}
		else if (parameter.c_str()[0] == '\"') {
			word = parameter;
		}
		else {
			filename = parameter;
		}
	}

	if (!filename.empty()) {
		auto [ file_handle, error ] = kiv_os_rtl::Open_File(filename, utils::get_file_attrs());
		if (error != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(error);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return 2;
		}
	}
	else {
		file_handle = stdin_handle;
	}


	while (true) {
		auto [chars_read, error] = kiv_os_rtl::Read_File(file_handle, buffer);
		if (chars_read > 0) {
			file_content.append(buffer.data()).append(new_line);
			buffer.fill('\0');
		}
		else {
			break;
		}
		//kiv_os_rtl::Write_File(stdin_handle, new_line);
	}

	error = kiv_os_rtl::Close_Handle(file_handle);
	if (error != kiv_os::NOS_Error::Success && !filename.empty()) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}


	word = word.substr(1, word.size() - 2);
	if (word.empty()) {
		kiv_os_rtl::Write_File(file_handle, ss.str().data(), ss.str().size(), chars_written);
		kiv_os_rtl::Exit(0);
		return 0;
	}

	std::istringstream input(file_content);
	std::ostringstream output;
	output << new_line;
	while (std::getline(input, line, '\n')) {
		if (!print_not_contain_lines) {
			if (line.find(word) != std::string::npos) {
				output << line << new_line;
			}
			line_counter++;
		}
		else {
			if (line.find(word) == std::string::npos) {
				output << line << new_line;
			}
			line_counter++;
		}

	}
	
	std::string find("FIND:");
	kiv_os_rtl::Write_File(stdout_handle, new_line.data(), new_line.size(), chars_written);
	kiv_os_rtl::Write_File(stdout_handle, find.data(), find.size(), chars_written);

	if (!print_count_lines) {
		kiv_os_rtl::Write_File(stdout_handle, output.str().data(), output.str().size(), chars_written);
	} else {
		std::string count_contain_lines = std::to_string(line_counter);
		kiv_os_rtl::Write_File(stdout_handle, count_contain_lines.data(), count_contain_lines.size(), chars_written);
	}

	kiv_os_rtl::Exit(0);
	return 0;
}
