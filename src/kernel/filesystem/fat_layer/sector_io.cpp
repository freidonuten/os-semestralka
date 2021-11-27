#include "sector_io.h"

#include <algorithm>

Sector_IO::Sector_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->disk_io = std::make_unique<Disk_IO>(disk, info);
	this->info = info;
}

void Sector_IO::Proceed_Task(std::shared_ptr<IO_Task> task) {
	auto sectors = task->data_blocks;
	auto start = task->starting_byte;
	auto buffer = task->buffer;
	size_t remaining = task->how_many_bytes_to_operate;

	std::uint64_t index = 0;
	std::uint64_t count = sectors.size();
	

	while (index < count) {
		std::uint64_t current_start = Calculate_Position(start, index);
		std::vector<std::uint64_t> current_sectors = Get_Consecutive_Sectors(sectors, index);
		std::uint64_t current_bytes_to_write = Calculate_How_Many(current_start, current_sectors, remaining);

		auto current_task = IO_Task_Factory::Make_Task(current_sectors, current_start, current_bytes_to_write, buffer, task->type);

		this->disk_io->Proceed_Task(current_task);

		index += current_sectors.size();
		remaining -= current_bytes_to_write;
		buffer = Add_To_Pointer(buffer, current_bytes_to_write);
	}
}


std::vector<std::uint64_t> Sector_IO::Get_Consecutive_Sectors(std::vector<std::uint64_t> source, std::uint64_t start_index) {
	std::vector<std::uint64_t> result;
	std::uint64_t count = source.size();

	for (std::uint64_t i = start_index; i < count; i++) {
		//either it's the first element
		//or the current element is one bigger than the last
		if (i == start_index || source[i] == source[i - 1] + 1) {
			result.push_back(source[i]);
		}
		else {
			break;
		}
	}

	return result;
}

std::uint64_t Sector_IO::Calculate_Position(std::uint64_t start, std::uint64_t index) {
	if (index == 0) {
		return start;
	}
	else {
		return this->info->Bytes_Per(Data_Block::SECTOR) * index;
	}
}

std::uint64_t Sector_IO::Calculate_How_Many(std::uint64_t current_start, std::vector<std::uint64_t> current_sectors, std::uint64_t remaining) {
	std::uint64_t count = current_sectors.size();

	std::uint64_t bytes_in_sectors = count * this->info->Bytes_Per(Data_Block::SECTOR);
	std::uint64_t bytes_to_write = bytes_in_sectors - current_start;

	return std::min(bytes_to_write, remaining);
}

void* Sector_IO::Add_To_Pointer(void* ptr, std::uint64_t how_many) {
	std::uint8_t* casted_ptr = static_cast<std::uint8_t*>(ptr);
	casted_ptr += how_many;
	return static_cast<void*>(casted_ptr);
}

