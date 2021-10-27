#include <Windows.h>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include "..\api\api.h"
#include "handles.h"
#include "Trigger.h"
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

Thread_Control_Block& Task_Manager::get_current_thread() {
	return get_thread(Thread_Control_Block::get_tid_of(std::this_thread::get_id()));
}

Thread_Control_Block& Task_Manager::get_thread(const kiv_os::THandle handle) {
	auto thread = std::find_if(thread_table.begin(), thread_table.end(),
		[handle](const auto& thread) {
			return thread.get_tid() == handle;
		}
	);
	
	// current thread doesn't exist, this is fatal
	if (thread == thread_table.end()) {
		throw std::runtime_error("Current thread doesn't exist, wtf!?");
	}

	return *thread;

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

const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs, Process_Control_Block& parent) {
	// find empty thread slot
	auto thread_slot = std::find_if(thread_table.begin(), thread_table.end(),
		[](const auto& tcb) {
			return tcb.get_state() == Execution_State::FREE;
		}
	);
	if (thread_slot == thread_table.end()) { // no slot available
		throw std::runtime_error("No empty thread slots are available");
	}

	// declare entry point
	const auto name_ptr = reinterpret_cast<char*>(regs.rdx.r);
	const auto entry = TThread_Proc(GetProcAddress(User_Programs, name_ptr));
	if (!entry) {
		throw std::runtime_error("Program not found");
	}

	// start the thread and assign it to parent process
	thread_slot->allocate(entry, regs);
	thread_slot->adopt(parent);

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs) {
	return create_thread(regs, get_current_process());
}

const NOS_Error Task_Manager::create_process(kiv_hal::TRegisters& regs) {
	// parse registers
	const auto stdin_handle = static_cast<THandle>(regs.rbx.e >> 16);
	const auto stdout_handle = static_cast<THandle>(regs.rbx.x);

	// alloc process
	auto &process = alloc_first_free();

	process.fd_insert(stdin_handle);
	process.fd_insert(stdout_handle);
	process.set_cwd(get_current_process().get_cwd());

	create_thread(regs, process);

	return NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::exit(kiv_hal::TRegisters& regs) {
	static const auto exit = [&regs](auto& object) {
		object.exit(regs.rcx.x);
	};

	auto& process = get_current_process();
	auto& thread = get_current_thread();
	const auto kill_process = process.is_main_thread(thread.get_tid());

	kill_process ? exit(process) : exit(thread);

	// FIXME could exit possibly result in a failure?
	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::register_signal_handler(kiv_hal::TRegisters& regs) {
	const auto signal = static_cast<kiv_os::NSignal_Id>(regs.rcx.r);
	const auto handler = reinterpret_cast<kiv_os::TThread_Proc>(regs.rdx.r);

	auto &current_thread = get_current_thread();
	if (!regs.rdx.r) {
		current_thread.remove_signal_handle(signal);
	} else {
		current_thread.register_signal_handle(signal, handler);
	}

	return kiv_os::NOS_Error::Success;
}

const NOS_Error Task_Manager::clone(kiv_hal::TRegisters& regs) {
	const auto object_type = static_cast<NClone>(regs.rcx.l);

	return [object_type, &regs, this]() {
		switch (object_type) {
			case NClone::Create_Process:
				return create_process(regs);
			case NClone::Create_Thread:
				return create_thread(regs);
			default:
				return NOS_Error::Invalid_Argument;
		}
	}();

}

const kiv_os::NOS_Error Task_Manager::wait_for(kiv_hal::TRegisters& regs) {
	const auto handles_begin = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
	const auto handles_end = handles_begin + regs.rcx.r;
	const auto find_finished = [handles_begin, handles_end, &regs, this]() {
		const auto finished_handle = std::find_if(handles_begin, handles_end,
			[this](const auto handle) {
				return get_thread(handle).get_state() == Execution_State::FINISHED;
			}
		);
		regs.rax.x = static_cast<uint16_t>(finished_handle - handles_begin);
		return finished_handle != handles_end;
	};

	if (find_finished()) {
		return kiv_os::NOS_Error::Success;
	}

	// all threads are working, initialize trigger
	Trigger trigger{}; 

	// insert trigger to each thread
	std::for_each(handles_begin, handles_end,
		[this, &trigger](const auto handle) {
			get_thread(handle).insert_exit_trigger(trigger);
		}
	);

	// and now wait for trigger
	trigger.wait();

	if (!find_finished()) {
		// if we're here, something fucked up real bad
		throw std::runtime_error("Process signaled but can't find signaling thread.");
	}

	return kiv_os::NOS_Error::Success;
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