#pragma once

#include "..\api\api.h"
#include "constants.h"
#include "Process_Control_Block.h"
#include "Thread_Control_Block.h"

#include <array>


class Task_Manager final {
	private:
		using Process_Table = std::array<Process_Control_Block, constants::process_limit>;
		using Thread_Table = std::array<Thread_Control_Block, constants::thread_limit>;

		
		Process_Table process_table;
		Thread_Table thread_table;

		// helper methods
		const kiv_os::NOS_Error create_process(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error create_thread(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error create_thread(kiv_hal::TRegisters& regs, Process_Control_Block& parent);
		Thread_Control_Block& get_current_thread();
		Thread_Control_Block& get_thread(const kiv_os::THandle handle);
		Process_Control_Block& get_current_process();
		Process_Control_Block& alloc_first_free();

		// syscall impl
		const kiv_os::NOS_Error clone(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error wait_for(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error read_exit_code(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error exit(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error shutdown(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error register_signal_handler(kiv_hal::TRegisters& regs);

	public:
		explicit Task_Manager();

		void syscall_dispatch(kiv_hal::TRegisters& regs);
};
