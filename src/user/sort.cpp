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

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	auto file_content = std::string("");
	auto buffer = std::array<char, BUFFER_SIZE>();
	for (size_t count = 1; count;) {
		std::tie(count, std::ignore) = rtl::Read_File(handle, buffer);
		file_content.append(buffer.data(), count);
	}

	if (kiv_os_rtl::Close_Handle(handle) != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle,  utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	auto file_lines = std::vector<std::string>();
	auto iss = std::istringstream(file_content);
	for (std::string line; std::getline(iss, line, '\n'); ) {
		file_lines.push_back(line);
	}

	auto result = std::string();
	const auto append_line = [&result](const auto& line) { result.append(line + '\n'); };

	std::sort(file_lines.begin(), file_lines.end());
	std::for_each(file_lines.begin(), file_lines.end(), append_line);

	rtl::Write_File(stdout_handle, result);
	KIV_OS_EXIT(0)
}
