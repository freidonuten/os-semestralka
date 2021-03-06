#pragma once

#include "../../api/hal.h"
#include "../../api/api.h"

#include "utils/global_constants.h"
#include "vfs_layer/vfs.h"


#include <memory>


namespace file_system {
	class Dispatcher final {
	private:
		VFS vfs;

	public:
		Dispatcher() = default;
		Dispatcher(uint16_t sector_size, uint64_t sector_count, int drive_id);

		void operator()(kiv_hal::TRegisters& regs);
	};

	Dispatcher factory();

	void close_handle(kiv_hal::TRegisters& regs, VFS& vfs);
	void create_pipe(kiv_hal::TRegisters& regs, VFS& vfs);
	void delete_file(kiv_hal::TRegisters& regs, VFS& vfs);
	void open_file(kiv_hal::TRegisters& regs, VFS& vfs);
	void write_file(kiv_hal::TRegisters& regs, VFS& vfs);
	void read_file(kiv_hal::TRegisters& regs, VFS& vfs);
	void seek(kiv_hal::TRegisters& regs, VFS& vfs);
	void get_file_attr(kiv_hal::TRegisters& regs, VFS& vfs);
	void get_cwd(kiv_hal::TRegisters& regs, VFS& vfs);
	void set_file_attr(kiv_hal::TRegisters& regs, VFS& vfs);
	void set_cwd(kiv_hal::TRegisters& regs, VFS& vfs);
}
