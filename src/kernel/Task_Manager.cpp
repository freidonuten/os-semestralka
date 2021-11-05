#include <Windows.h>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include "..\api\api.h"
#include "handles.h"
#include "Task_Manager.h"
#include "Kernel_Utils.h"
#include "Process_Control_Block.h"

#include "kernel.h"


extern HMODULE User_Programs;

Task_Manager::Task_Manager() 
	: process_table{ }
	, thread_table{ }
{
	for (auto i = 0; i < process_table.size(); ++i) {
		process_table[i] = Process_Control_Block(i);
	}
}

Thread_Control_Block& Task_Manager::get_current_thread() {
	return get_thread(Thread_Control_Block::current_tid());
}

Thread_Control_Block& Task_Manager::get_thread(const kiv_os::THandle handle) {
	const auto resolve = [this](const auto tid) -> Thread_Control_Block& {
		return process_table.at(thread_table.at(tid)).get_thread(tid);
	};

	return kut::is_proc(handle)
		? resolve(get_process(handle).get_tid())
		: resolve(handle);
}

Process_Control_Block& Task_Manager::get_current_process() {
	return get_process(get_current_thread().get_ppid());
}

Process_Control_Block& Task_Manager::get_process(const kiv_os::THandle handle) {
	return process_table.at(handle);
}

Process_Control_Block& Task_Manager::alloc_first_free() {
	const auto process_slot = std::find_if(
		process_table.begin(), process_table.end(), kut::is_free<Process_Control_Block>
	);

	// this is possibly fatal, we ran out of process slots
	if (process_slot == process_table.cend()) {
		throw std::runtime_error("Out of PCB slots");
	}

	process_slot->allocate();

	return *process_slot;
}

template<bool return_tid>
const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs, Process_Control_Block& parent) {
	// declare entry point
	const auto name_ptr = reinterpret_cast<char*>(regs.rdx.r);
	const auto entry = kiv_os::TThread_Proc(GetProcAddress(User_Programs, name_ptr));

	if (!entry) {
		return kiv_os::NOS_Error::File_Not_Found;
	}

	const auto tid = parent.thread_insert(entry, regs);
	const auto pid = parent.get_pid();

	thread_table.emplace(tid, pid);
	regs.rax.x = return_tid ? tid : pid;

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs) {
	return create_thread<true>(regs, get_current_process());
}

const kiv_os::NOS_Error Task_Manager::create_process(kiv_hal::TRegisters& regs) {
	// alloc process
	auto &process = alloc_first_free();
	auto child_regs = kiv_hal::TRegisters{ };

	child_regs.rax.x =  static_cast<kiv_os::THandle>(regs.rbx.e >> 16);
	child_regs.rbx.x =  static_cast<kiv_os::THandle>(regs.rbx.x);

	create_thread<false>(child_regs, process);

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::exit(kiv_hal::TRegisters& regs) {
	static const auto exit = [&regs](auto& object) {
		object.exit(regs.rcx.x);
	};

	auto& process = get_current_process();
	auto& thread = get_current_thread();
	const auto exit_process = process.is_main_thread(thread.get_tid());

	exit_process ? exit(process) : exit(thread);

	// FIXME could exit possibly result in a failure?
	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::shutdown(kiv_hal::TRegisters& regs) {
	std::for_each(process_table.begin(), process_table.end(), [](auto& process) {
		process.terminate();
	});

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::register_signal_handler(kiv_hal::TRegisters& regs) {
	const auto signal = static_cast<kiv_os::NSignal_Id>(regs.rcx.r);
	const auto handler = reinterpret_cast<kiv_os::TThread_Proc>(regs.rdx.r);

	get_current_thread().register_signal_handle(handler);

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::clone(kiv_hal::TRegisters& regs) {
	switch (static_cast<kiv_os::NClone>(regs.rcx.l)) {
		case kiv_os::NClone::Create_Process:
			return create_process(regs);
		case kiv_os::NClone::Create_Thread:
			return create_thread(regs);
	}

	return kiv_os::NOS_Error::Invalid_Argument;
}

const kiv_os::NOS_Error Task_Manager::wait_for(kiv_hal::TRegisters& regs) {
	// there's a hard limit on object count
	if (regs.rcx.e > constants::wait_obj_limit) {
		return kiv_os::NOS_Error::Out_Of_Memory;
	}

	const auto handles = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
	const auto count = regs.rcx.e;

	// remap userspace handles (tids/pids) to native handles
	std::array<HANDLE, constants::wait_obj_limit> native_handles;
	std::for_each(handles, handles + count,
		[it = native_handles.begin(), this](const auto handle) mutable {
			*it++ = get_thread(handle).get_native_handle();
		}
	);

	// block until first signal
	regs.rax.r = WaitForMultipleObjects(static_cast<DWORD>(count), native_handles.data(), false, INFINITE);

	return (regs.rax.x == INFINITE)
		? kiv_os::NOS_Error::Unknown_Error
		: kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::read_exit_code(kiv_hal::TRegisters& regs) {
	auto& thread = get_thread(static_cast<kiv_os::THandle>(regs.rdx.x));
	
	regs.rcx.x = thread.read_exit_code();
	thread_table.erase(thread.get_tid());

	return kiv_os::NOS_Error::Success;
}

void Task_Manager::syscall_dispatch(kiv_hal::TRegisters& regs) {
	const auto return_code = [&regs, this]() {
		switch (static_cast<kiv_os::NOS_Process>(regs.rax.l)) {
			case kiv_os::NOS_Process::Clone:
				return clone(regs);
			case kiv_os::NOS_Process::Wait_For:
				return wait_for(regs);
			case kiv_os::NOS_Process::Read_Exit_Code:
				return read_exit_code(regs);
			case kiv_os::NOS_Process::Exit:
				return exit(regs);
			case kiv_os::NOS_Process::Shutdown:
				return shutdown(regs);
			case kiv_os::NOS_Process::Register_Signal_Handler:
				return register_signal_handler(regs);
		}

		return kiv_os::NOS_Error::Invalid_Argument;
	}();

	Set_Error(return_code, regs);
}