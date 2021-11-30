#include "freg.h"

void Update_Freq_Table(std::array<unsigned int, freq_table_size>& freq_table, const char* buffer, size_t &chars_read) {
	unsigned int character;
	for (int i = 0; i < chars_read; i++) {
		character = static_cast<unsigned int>(*(buffer + i));
		freq_table[character]++;
	}
}

size_t __stdcall freq(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	size_t chars_written = 0;
	size_t chars_read = 1;
	char buffer[BUFFER_SIZE];
	std::stringstream ss;
	std::array<unsigned int, freq_table_size> freq_table;
	kiv_os::NOS_Error error;

	freq_table.fill(0);

	while (chars_read) {
		memset(buffer, 0, BUFFER_SIZE);
		chars_read = 0;
		error = kiv_os_rtl::Read_File(stdin_handle, buffer, BUFFER_SIZE, chars_read);
		if (error != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(error);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return 2;
		}
		if (buffer[0] == eof ||
			buffer[0] == eot ||
			buffer[0] == etx) {
			break;
		}
		Update_Freq_Table(freq_table, buffer, chars_read);
	}

	error = kiv_os_rtl::Write_File(stdout_handle, new_line.data(), new_line.size(), chars_written);
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}
	memset(buffer, 0, BUFFER_SIZE);

	for (int i = 0; i < freq_table_size; i++) {
		unsigned int counter = freq_table[i];
		if (counter != 0) {
			chars_written = sprintf_s(buffer, "0x%hhx : %d\n", i, counter);
			ss << buffer;
		}
	}
	kiv_os_rtl::Write_File(stdout_handle, ss.str().data(), ss.str().size(), chars_written);
	kiv_os_rtl::Write_File(stdout_handle, new_line.data(), new_line.size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
