#include "rgen.h"
#include <random>

bool is_eof = false;
bool thread_terminated = false;

size_t Terminated(const kiv_hal::TRegisters& regs) {
	thread_terminated = true;
	return 0;
}

size_t Check_EOF(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	uint8_t buffer[eof_buffer_size];
	size_t chars_read = 1;

	while (chars_read) {
		kiv_os_rtl::Read_File(stdin_handle, (char*)buffer, eof_buffer_size, chars_read);
	}

	is_eof = true;
	kiv_os_rtl::Exit(0);
	return 0;
}

size_t __stdcall rgen(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle process_handle;
	kiv_os::THandle stdin_handle = regs.rax.x;
	kiv_os::THandle stdout_handle = regs.rbx.x;
	std::string float_num = "";
	size_t chars_written = 0;
	uint16_t exit_code;		 

	is_eof = false;

	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc thread_handle = reinterpret_cast<kiv_os::TThread_Proc>(Terminated);

	kiv_os_rtl::Register_Signal_Handler(signal, thread_handle);


	std::random_device rd;
	std::default_random_engine engine(rd());
	std::uniform_real_distribution<float> dist(0, 1);

	kiv_os_rtl::Create_Thread(&Check_EOF, &is_eof, stdin_handle, stdout_handle, process_handle);

	while (!is_eof && !thread_terminated) {
		float_num = std::to_string(dist(engine));
		float_num.append("\n");
		kiv_os_rtl::Write_File(stdout_handle, float_num.data(), float_num.size(), chars_written);
		float_num.clear();
	}

	exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}
