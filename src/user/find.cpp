#include "find.h"
#include "utils.h"

size_t __stdcall find(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	kiv_os::THandle stdin_handle = regs.rax.x;
	kiv_os::THandle stdout_handle = regs.rbx.x;
	const char* parameters = reinterpret_cast<const char*>(regs.rdi.r);
	size_t chars_written = 0;
	size_t chars_read = 1;
	size_t offset = 0;
	size_t line_counter = 0;
	char buffer[BUFFER_SIZE];
	std::string line;
	std::string word;
	std::string first_parameter;
	std::string second_paramer;
	std::string filename;
	bool print_count_lines = false;
	bool print_not_contain_lines = false;
	kiv_os::NOS_Error error;

	if (strlen(parameters) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::stringstream ss(parameters);

	ss >> first_parameter;
	ss >> second_paramer;
	ss >> word;
	ss >> filename;

	if (first_parameter == "/V" || first_parameter == "/v" ||
		second_paramer == "/V" || second_paramer == "/v") {
		print_not_contain_lines = true;
	}

	if (first_parameter == "/C" || first_parameter == "/c" ||
		second_paramer == "/C" || second_paramer == "/c") {
		print_count_lines = true;
	}

	error = kiv_os_rtl::Open_File(filename.data(), utils::get_file_attrs(), kiv_os::NOpen_File::fmOpen_Always, file_handle);
	
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}
	
	while (chars_read) {
		error = kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, offset);
		if (error != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(error);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return 2;
		}
		memset(buffer, 0, BUFFER_SIZE);
		chars_read = 0;
		error = kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		if (error != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(error);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return 2;
		}
		ss << buffer;
		offset += chars_read;
	}
	
	error = kiv_os_rtl::Close_Handle(file_handle);
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	if (word.empty()) {
		kiv_os_rtl::Write_File(file_handle, ss.str().data(), ss.str().size(), chars_written);
		kiv_os_rtl::Exit(0);
		return 0;
	}

	std::ostringstream output;
	

	while (std::getline(ss, line, '\n')) {
		if (!print_not_contain_lines) {
			if (line.find(word) != std::string::npos) {
				output << line << new_line;
			}
			line_counter++;
		} else {
			if (line.find(word) == std::string::npos) {
				output << line << new_line;
			}
			line_counter++;
		}
		
	}
	
	kiv_os_rtl::Write_File(stdout_handle, output.str().data(), output.str().size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
