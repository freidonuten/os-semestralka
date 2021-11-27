#pragma once

#include "kernel.h"
#include "filesystem/test.h"
#include "Task_Manager.h"
#include <Windows.h>
#include "filesystem/cwd/cwd.h"

#include <memory>
#include "filesystem/vfs_layer/vfs.h"
#include "filesystem/file_system.h"


HMODULE User_Programs;
auto task_manager = Task_Manager();
std::unique_ptr<file_system::Dispatcher> fs_dispatch;


void Initialize_Kernel() {
	User_Programs = LoadLibraryW(L"user.dll");
}

void Shutdown_Kernel() {
	FreeLibrary(User_Programs);
}

void __stdcall Sys_Call(kiv_hal::TRegisters &regs) {
	switch (static_cast<kiv_os::NOS_Service_Major>(regs.rax.h)) {
		case kiv_os::NOS_Service_Major::File_System:		
			return (*fs_dispatch)(regs);
		case kiv_os::NOS_Service_Major::Process:
			return task_manager.syscall_dispatch(regs);
	}
}

void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context) {
	Initialize_Kernel();
	kiv_hal::Set_Interrupt_Handler(kiv_os::System_Int_Number, Sys_Call);
	kiv_hal::TRegisters regs;

	
	for (regs.rdx.l = 0; ; regs.rdx.l++) {
		kiv_hal::TDrive_Parameters params;		
		regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Drive_Parameters);;
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&params);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
			
		if (!regs.flags.carry) {
				uint16_t sector_size = params.bytes_per_sector;
				uint64_t sector_count = params.absolute_number_of_sectors;
				int drive_id = regs.rdx.l;

				fs_dispatch = std::make_unique<file_system::Dispatcher>(sector_size, sector_count, drive_id);
				break;
		}

		if (regs.rdx.l == 255) break;
	}
	
	filesystem_test();
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