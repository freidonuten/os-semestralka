#include "sort.h"
#include "utils.h"
#include <array>

size_t __stdcall sort(const kiv_hal::TRegisters& regs) {
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto stdin_handle  = kiv_os::THandle(regs.rax.x);
	const auto filename = std::string_view(reinterpret_cast<const char*>(regs.rdi.r));

	const auto [handle, error] = filename.size()
		? kiv_os_rtl::Open_File(filename, utils::get_file_attrs())
		: std::pair{ stdin_handle, kiv_os::NOS_Error::Success };

	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	auto file_content = std::string("");
	auto buffer = std::array<char, BUFFER_SIZE>();
	while (true) {
		const auto [count, error] = kiv_os_rtl::Read_File(handle, buffer);
		if (!count > 0) {
			break;
		}
		file_content.append(buffer.data(), count);
	}

	if (kiv_os_rtl::Close_Handle(handle) != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle,  utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	auto file_lines = std::vector<std::string>(
		std::istream_iterator<std::string>{std::istringstream{file_content}},
		std::istream_iterator<std::string>()
	);

	auto result = std::string();
	const auto append_line = [&result](const auto& line) { result.append(line + '\n'); };

	std::sort(file_lines.begin(), file_lines.end());
	std::for_each(file_lines.begin(), file_lines.end(), append_line);

	kiv_os_rtl::Write_File(stdout_handle, result);
	KIV_OS_EXIT(0)
}
