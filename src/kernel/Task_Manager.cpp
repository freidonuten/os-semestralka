﻿#include <Windows.h>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include "..\api\api.h"
#include "handles.h"
#include "Task_Manager.h"

#include "kernel.h"


extern HMODULE User_Programs;

using kiv_os::NOS_Process;
using kiv_os::NOS_Error;
using kiv_os::TThread_Proc;
using kiv_os::THandle;
using kiv_os::NClone;


Task_Manager::Task_Manager() 
	: process_table{ }
	, thread_table{ }
{
	for (auto i = 0; i < process_table.size(); ++i) {
		process_table[i] = Process_Control_Block(i);
	}
}

Process_Control_Block& Task_Manager::get_current_process() {
	// find thread instance with matching tid (linear search)
	auto current_thread = std::find_if(thread_table.begin(), thread_table.end(),
		[](const auto& tcb) {
			return tcb.is_current();
		}
	);

	// this is fatal, just throw...
	if (current_thread == thread_table.end()) {
		throw std::runtime_error("Running proces doesn't exist?");
	}

	return process_table.at(current_thread->get_ppid());
}

Process_Control_Block& Task_Manager::alloc_first_free() {
	const auto process_slot = std::find_if(process_table.begin(), process_table.end(),
		[](const auto& a) {
			return a.get_state() == Execution_State::FREE;
		}
	);

	// this is possibly fatal, we ran out of process slots
	if (process_slot != process_table.cend()) {
		throw std::runtime_error("Out of PCB slots");
	}

	process_slot->allocate();

	return *process_slot;
}

Thread_Control_Block& Task_Manager::create_thread(const char* program, const kiv_hal::TRegisters& context) {
	auto thread_slot = std::find_if(thread_table.begin(), thread_table.end(),
		[](const auto& tcb) {
			return tcb.get_state() == Execution_State::FREE;
		}
	);

	if (thread_slot == thread_table.end()) {
		throw std::runtime_error("No empty thread slots are available");
	}

	const auto entry = TThread_Proc(GetProcAddress(User_Programs, program));
	if (!program) {
		throw std::runtime_error("Program not found");
	}

	thread_slot->allocate(entry, context);

	return *thread_slot;
}

const NOS_Error Task_Manager::create_process(kiv_hal::TRegisters& regs) {
	// parse registers
	const auto name_ptr = reinterpret_cast<char*>(regs.rdx.r);
	// we might not need args at this point, it stays in the registers
	// and executed process will interpret it on its own...?
	//const auto args_ptr = reinterpret_cast<char*>(regs.rdi.r);
	const auto stdin_handle = static_cast<THandle>(regs.rbx.e >> 16);
	const auto stdout_handle = static_cast<THandle>(regs.rbx.x);

	// alloc process
	auto process = alloc_first_free();

	process.fd_insert(stdin_handle);
	process.fd_insert(stdout_handle);
	process.set_cwd(get_current_process().get_cwd());
	create_thread(name_ptr, regs).adopt(process);

	return NOS_Error::Success;
}

const NOS_Error Task_Manager::clone(kiv_hal::TRegisters& regs) {
	const auto object_type = static_cast<NClone>(regs.rcx.l);

	return [object_type, &regs, this]() {
		switch (object_type) {
			case NClone::Create_Process:
				return create_process(regs);
			case NClone::Create_Thread:
				return NOS_Error::Unknown_Error; // TODO implement
			default:
				return NOS_Error::Unknown_Error;
				// TODO handle failure
		}
	}();

}

void Task_Manager::syscall_dispatch(kiv_hal::TRegisters& regs) {
	const auto syscall_id = static_cast<NOS_Process>(regs.rax.l);
	const auto return_code = [syscall_id, &regs, this]() -> NOS_Error {
		switch (syscall_id) {
			case NOS_Process::Clone:
				return clone(regs);
			case NOS_Process::Wait_For:
				return wait_for(regs);
			case NOS_Process::Read_Exit_Code:
				return read_exit_code(regs);
			case NOS_Process::Exit:
				return exit(regs);
			case NOS_Process::Shutdown:
				return shutdown(regs);
			case NOS_Process::Register_Signal_Handler:
				return register_signal_handler(regs);
			default: // TODO handle failure (how?)
				return NOS_Error::Unknown_Error;
		}
	}();

	// TODO do something with the return code (what?)
}