#pragma once

#include "..\api\api.h"
#include "constants.h"
#include "Process_Control_Block.h"
#include "Thread_Control_Block.h"

#include <unordered_map>
#include <array>


class Task_Manager final {
	public:
		using Process_Table = std::array<Process_Control_Block, constants::process_limit>;
		using Thread_Table = std::unordered_map<kiv_os::THandle, kiv_os::THandle>;

	private:
		Process_Table process_table;
		Thread_Table thread_table;
		Dummy_CWD_Holder* cwd_holder = nullptr;
		HANDLE shutdown_event;

		// helper methods
		bool is_valid_handle(kiv_os::THandle handle) const;
		const kiv_os::NOS_Error create_process(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error create_thread(kiv_hal::TRegisters& regs);
		std::tuple<kiv_os::THandle, kiv_os::NOS_Error> create_thread(kiv_hal::TRegisters& regs, Process_Control_Block& parent, bool exec_program);
		Thread_Control_Block* get_current_thread();
		Thread_Control_Block* get_thread(const kiv_os::THandle handle);
		Process_Control_Block* get_process(const kiv_os::THandle handle);
		std::pair<Process_Control_Block&, kiv_os::NOS_Error> alloc_first_free();
		void clean_up_thread_handle(const kiv_os::THandle handle);

		// syscall impl
		const kiv_os::NOS_Error clone(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error wait_for(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error read_exit_code(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error exit(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error shutdown(kiv_hal::TRegisters& regs);
		const kiv_os::NOS_Error register_signal_handler(kiv_hal::TRegisters& regs);

	public:
		explicit Task_Manager();

		Process_Control_Block* get_current_process();
		void inject_cwd_holder(Dummy_CWD_Holder* cwd_holder);
		const Process_Table& get_processes() const;
		void syscall_dispatch(kiv_hal::TRegisters& regs);
};
