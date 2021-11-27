#pragma once

#include "kernel.h"
#include "filesystem/test.h"
#include <Windows.h>
#include "filesystem/cwd/cwd.h"

#include <memory>
#include "filesystem/vfs_layer/vfs.h"
#include "filesystem/file_system.h"

HMODULE User_Programs;
Task_Manager task_manager{};
auto fs_dispatch = file_system::Dispatcher();


void Initialize_Kernel() {
	User_Programs = LoadLibraryW(L"user.dll");
}

void Shutdown_Kernel() {
	FreeLibrary(User_Programs);
}

void __stdcall Sys_Call(kiv_hal::TRegisters &regs) {
	switch (static_cast<kiv_os::NOS_Service_Major>(regs.rax.h)) {
		case kiv_os::NOS_Service_Major::File_System:		
			return fs_dispatch(regs);
		case kiv_os::NOS_Service_Major::Process:
			return task_manager.syscall_dispatch(regs);
	}
}

void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context) {
	Initialize_Kernel();
	kiv_hal::Set_Interrupt_Handler(kiv_os::System_Int_Number, Sys_Call);
	//filesystem_test();
	//return;
	kiv_hal::TRegisters regs;

	//v ramci ukazky jeste vypiseme dostupne disky

	char* shell = "shell";

	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::Process);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_Process::Clone);
	regs.rcx.l = static_cast<uint8_t>(kiv_os::NClone::Create_Process);
	regs.rdx.r = reinterpret_cast<uint64_t>(shell);
	regs.rbx.e = 1;

	Sys_Call(regs); // Clone shell

	kiv_os::THandle pid = regs.rax.x;

	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::Process);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For);
	regs.rcx.r = 1;
	regs.rdx.r = reinterpret_cast<uint64_t>(&pid);

	Sys_Call(regs); // Wait for shell

	Shutdown_Kernel();
}


void Set_Error(const kiv_os::NOS_Error error, kiv_hal::TRegisters &regs) {
	if (error == kiv_os::NOS_Error::Success) {
		regs.flags.carry = false;
	} else {
		regs.flags.carry = true;
		regs.rax.r = static_cast<uint64_t>(error);
	}
}