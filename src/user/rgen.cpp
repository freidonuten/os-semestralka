#include "rgen.h"
#include <random>
#include <array>


bool is_eof = false;
bool thread_terminated = false;

size_t Terminated(const kiv_hal::TRegisters& regs) {
	thread_terminated = true;
	return 0;
}

size_t Check_EOF(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	auto buffer = std::array<char, eof_buffer_size>();

	for (size_t count = 1; count; ) {
		std::tie(count, std::ignore) = kiv_os_rtl::Read_File(stdin_handle, buffer);
	}

	is_eof = true;
	KIV_OS_EXIT(0)
}

size_t __stdcall rgen(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto signal_handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated);

	is_eof = false;
	thread_terminated = false;

	kiv_os_rtl::Register_Signal_Handler(kiv_os::NSignal_Id::Terminate, signal_handler);

	kiv_os::THandle thread_handle;
	const auto error = kiv_os_rtl::Create_Thread(&Check_EOF, &is_eof, stdin_handle, stdout_handle, thread_handle);

	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(1)
	}

	std::random_device rd;
	std::default_random_engine engine(rd());
	std::uniform_real_distribution<float> dist(0, 1);

	while (!is_eof && !thread_terminated) {
		const auto float_num = std::to_string(dist(engine)) + '\n';
		kiv_os_rtl::Write_File(stdout_handle, float_num);
	}

	uint16_t code;
	kiv_os_rtl::Read_Exit_Code(thread_handle, code);
	KIV_OS_EXIT(0);
}
