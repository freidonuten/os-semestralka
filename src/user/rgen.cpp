#include "rgen.h"
#include "rtl_wrappers.h"
#include <random>
#include <array>


bool is_eof = false;
bool thread_terminated = false;

size_t Terminated(const kiv_hal::TRegisters& regs) {
	// Terminate thread
	thread_terminated = true;
	return 0;
}

// Checking if eof is recieved
size_t Check_EOF(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	auto buffer = std::array<char, eof_buffer_size>{ };

	// read until "stop char" is encountered
	do {
		// Load content to buffer
		const auto [size, eof, error] = rtl::Read_File(stdin_handle, buffer);
		// end of stream
		if (eof) {
			break; 
		}
	} while (!utils::is_stop_char(buffer[0]));

	is_eof = true;
	KIV_OS_EXIT(0)
}

size_t __stdcall rgen(const kiv_hal::TRegisters& regs) {
	const auto stdin_handle = kiv_os::THandle(regs.rax.x);
	const auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	const auto signal_handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated);

	// initialize termination flags to false
	is_eof = thread_terminated = false;

	// register new signal to terminate
	rtl::Register_Signal_Handler(kiv_os::NSignal_Id::Terminate, signal_handler);

	// Create new thred that will check if eof is recieved
	const auto [thread_handle, error] = rtl::Create_Thread(&Check_EOF, &is_eof, stdin_handle, stdout_handle);

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(1)
	}

	std::random_device rd;
	std::default_random_engine engine(rd());
	std::uniform_real_distribution<float> dist(0, 1);

	// Write random number to output until thread is terminate or eof recieved
	while (!is_eof && !thread_terminated) {
		// generate random numbers and keep printing them until eof detected
		rtl::Write_File(stdout_handle, std::to_string(dist(engine)) + '\n');
	}

	rtl::Read_Exit_Code(thread_handle);
	KIV_OS_EXIT(0);
}
