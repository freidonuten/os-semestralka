#include "freg.h"
#include "rtl_wrappers.h"
#include <iomanip>
#include <array>


size_t __stdcall freq(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	auto buffer = std::array<char, BUFFER_SIZE>{};
	auto freq_table = std::array<unsigned int, freq_table_size>{0};
	const auto update_freq = [&freq_table](const auto c) {
		++freq_table[static_cast<unsigned int>(c)];
	};

	while (1) {
		const auto [size, error] = rtl::Read_File(stdin_handle, buffer);
		if (error != kiv_os::NOS_Error::Success) {
			rtl::Write_File(stdout_handle, utils::get_error_message(error));
			KIV_OS_EXIT(2);
		}

		std::for_each(buffer.begin(), buffer.begin() + size, update_freq);

		if (!size || utils::is_stop_char(buffer[size - 1])) {
			break;
		}
	}

	const auto [size, error] = rtl::Write_File(stdout_handle, new_line);
	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(3);
	}

	auto ss = std::ostringstream();
	const auto print = [&ss, i = 0](const auto c) mutable {
		if (c) {
			ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << i << " : " << std::dec << c << '\n';
		} ++i; 
	};

	std::for_each(freq_table.begin(), freq_table.end(), print);
	rtl::Write_File(stdout_handle, ss.str());
	KIV_OS_EXIT(0);
}
