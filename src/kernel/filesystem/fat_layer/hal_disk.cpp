#include "disk.h"
#include "../../../api/hal.h"

Hal_Disk::Hal_Disk(int drive_id) {
	this->drive_id = drive_id;
}

void Do_Operation(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index, int drive_id, kiv_hal::NDisk_IO operation) {
	kiv_hal::TDisk_Address_Packet dap;
	dap.count = sector_count;
	dap.lba_index = lba_index;
	dap.sectors = sectors_buffer;


	kiv_hal::TRegisters regs;
	regs.rdx.l = drive_id;
	regs.rax.h = static_cast<uint8_t>(operation);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&dap);


	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
}

void Hal_Disk::Write(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) {
	Do_Operation(sector_count, sectors_buffer, lba_index, this->drive_id, kiv_hal::NDisk_IO::Write_Sectors);
}

void Hal_Disk::Read(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) {
	Do_Operation(sector_count, sectors_buffer, lba_index, this->drive_id, kiv_hal::NDisk_IO::Read_Sectors);
}