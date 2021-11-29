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
	// initialize kernel process and insert placeholder thread
	const auto current_tid = Thread_Control_Block::current_tid();
	process_table[0] = Process_Control_Block{ 0, current_tid };
	process_table[0].set_name("kernel");
	thread_table[current_tid] = 0;

	// create shutdown event
	shutdown_event = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		TEXT("shutdown")
	);

	// initialize the rest
	std::generate(process_table.begin() + 1, process_table.end(), [i = 1]() mutable { return i++; });
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

void Task_Manager::inject_cwd_holder(Dummy_CWD_Holder* cwd_holder) {
	this->cwd_holder = cwd_holder;
}

Process_Control_Block& Task_Manager::get_process(const kiv_os::THandle handle) {
	return process_table.at(handle);
}

std::pair<Process_Control_Block&, kiv_os::NOS_Error> Task_Manager::alloc_first_free() {
	const auto process_slot = std::find_if(
		process_table.begin(), process_table.end(), kut::is_free<Process_Control_Block>
	);

	// this is possibly fatal, we ran out of process slots
	if (process_slot == process_table.cend()) {
		return { process_table[0], kiv_os::NOS_Error::Out_Of_Memory };
	}

	process_slot->allocate();

	return { *process_slot, kiv_os::NOS_Error::Success };
}

void Task_Manager::clean_up_thread_handle(const kiv_os::THandle handle) {
	auto& process = get_process(thread_table[handle]);

	if (process.get_tid() == handle) {
		// the dirty way...
		for (auto itr = thread_table.cbegin(); itr != thread_table.cend(); ) {
			itr = (itr->second == process.get_pid())
				? thread_table.erase(itr)
				: std::next(itr);
		}

		process.free();
		return;
	}

	thread_table.erase(handle);
	process.thread_remove(handle);
}

template<bool return_tid, typename result_type>
const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs, Process_Control_Block& parent, result_type& result) {
	const auto name_ptr = reinterpret_cast<char*>(regs.rdx.r);
	const auto entry = return_tid 
		? kiv_os::TThread_Proc(regs.rdx.r)
		: kiv_os::TThread_Proc(GetProcAddress(User_Programs, reinterpret_cast<char*>(regs.rdx.r)));

	if (!entry) {
		// TODO: zničit proces
		return kiv_os::NOS_Error::File_Not_Found;
	}

	const auto tid = parent.thread_insert(entry, regs);
	const auto pid = parent.get_pid();

	thread_table.emplace(tid, pid);
	result = return_tid ? tid : pid;

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::create_thread(kiv_hal::TRegisters& regs) {
	auto child_regs = kiv_hal::TRegisters{ regs };

	child_regs.rax.x = static_cast<kiv_os::THandle>(regs.rbx.e >> 16);
	child_regs.rbx.x = static_cast<kiv_os::THandle>(regs.rbx.x);

	return create_thread<true>(child_regs, get_current_process(), regs.rax.r);
}

const kiv_os::NOS_Error Task_Manager::create_process(kiv_hal::TRegisters& regs) {
	auto [process, error] = alloc_first_free();

	if (error != kiv_os::NOS_Error::Success) {
		return error;
	}

	process.set_args(reinterpret_cast<char*>(regs.rdi.r));
	process.set_name(reinterpret_cast<char*>(regs.rdx.r));

	auto child_regs = kiv_hal::TRegisters{ regs };

	child_regs.rax.x = static_cast<kiv_os::THandle>(regs.rbx.e >> 16);
	child_regs.rbx.x = static_cast<kiv_os::THandle>(regs.rbx.x);
	child_regs.rdi.r = reinterpret_cast<uint64_t>(process.get_args());

	cwd_holder->Inherit(get_current_process().get_pid(), process.get_pid());

	const auto result = create_thread<false>(child_regs, process, regs.rax.x);

	if (result != kiv_os::NOS_Error::Success) {
		process.terminate();
	}

	return result;
}

const kiv_os::NOS_Error Task_Manager::exit(kiv_hal::TRegisters& regs) {
	auto& process = get_current_process();
	auto& thread = get_current_thread();
	const auto exit_process = process.is_main_thread(thread.get_tid());

	thread.exit(regs.rcx.x);

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::shutdown(kiv_hal::TRegisters& regs) {
	SetEvent(shutdown_event);

	std::for_each(process_table.begin() + 1, process_table.end(), [](auto& process) {
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

	// if shutdown is signalized unblock everything
	native_handles[count] = shutdown_event;

	// block until first signal or shutdown
	const auto finished_index = WaitForMultipleObjects(static_cast<DWORD>(count + 1), native_handles.data(), false, INFINITE);

	if (finished_index == INFINITE) {
		kiv_os::NOS_Error::Unknown_Error;
	}
	
	//clean_up_thread_handle(handles[finished_index]);

	regs.rax.r = finished_index;

	return kiv_os::NOS_Error::Success;
}

const kiv_os::NOS_Error Task_Manager::read_exit_code(kiv_hal::TRegisters& regs) {
	auto& thread = get_thread(static_cast<kiv_os::THandle>(regs.rdx.x));
	const auto tid = thread.get_tid();
	
	std::array<HANDLE, 2> wait_handles = { thread.get_native_handle(), shutdown_event };
	const auto index = WaitForMultipleObjects(2, wait_handles.data(), false, INFINITE);
	
	regs.rcx.x = thread.read_exit_code();

	switch (index) {
	case INFINITE:
		return kiv_os::NOS_Error::Unknown_Error;
	case 0:
		clean_up_thread_handle(tid);
	}

	return kiv_os::NOS_Error::Success;
}

const Task_Manager::Process_Table& Task_Manager::get_processes() const {
	return process_table;
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