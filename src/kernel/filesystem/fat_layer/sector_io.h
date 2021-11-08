#pragma once

#include "disk.h"
#include "filesystem_info.h"
#include "disk_io.h"
#include "../utils/global_structs.h"

#include <memory>

class Sector_IO {
private:
	std::unique_ptr<Disk_IO> disk_io;
	std::shared_ptr<Filesystem_Info> info;

	std::vector<std::uint64_t> Get_Consecutive_Sectors(std::vector<std::uint64_t> source, int start_index);
	std::uint64_t Calculate_Position(std::uint64_t start, int index);
	size_t Calculate_How_Many(std::uint64_t current_start, std::vector<std::uint64_t> current_sectors, size_t remaining);
	void* Add_To_Pointer(void* ptr, std::uint64_t how_many);
 public:
	Sector_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	void Proceed_Task(std::shared_ptr<IO_Task> task);
};