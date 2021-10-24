#include <Windows.h>
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