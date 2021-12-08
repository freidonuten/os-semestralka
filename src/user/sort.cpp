#include "sort.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>

size_t __stdcall sort(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto stdin_handle  = kiv_os::THandle(regs.rax.x);
	const auto filename = std::string_view(reinterpret_cast<const char*>(regs.rdi.r));

	const auto [handle, error] = filename.size()
		? rtl::Open_File(filename, utils::get_file_attrs())
		: std::pair{ stdin_handle, kiv_os::NOS_Error::Success };

	const bool reading_from_console = handle == 0;

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	auto file_content = std::string("");
	auto buffer = std::array<char, BUFFER_SIZE>();
	while (1) {
		auto [count, eof, error] = rtl::Read_File(handle, buffer);
		file_content.append(buffer.data(), count);
		if (eof) {
			break;
		}
	}

	if (!reading_from_console) {
		rtl::Close_Handle(handle);
	}

	auto file_lines = std::vector<std::string>();
	auto iss = std::istringstream(file_content);
	for (std::string line; std::getline(iss, line, '\n'); ) {
		if (!line.size()) {
			continue;
		}
		file_lines.push_back(line);
	}

	const auto write = [stdout_handle](const auto& line) { rtl::Write_File(stdout_handle, line + '\n'); };

	std::sort(file_lines.begin(), file_lines.end());
	std::for_each(file_lines.begin(), file_lines.end(), write);

	KIV_OS_EXIT(0)
}
