#include "disk_io.h"

Disk_IO::Disk_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->disk = disk;
	this->info = info;
}

void Disk_IO::Proceed_Task(std::shared_ptr<IO_Task> task) {
	std::unique_ptr<Concrete_Disk_IO> concrete_disk_io;

	if (Are_Old_Sectors_Needed(task->data_blocks, task->starting_byte, task->how_many_bytes_to_operate) == true) {
		concrete_disk_io = std::make_unique<Via_Buffer_Disk_IO>(this->disk, this->info);
	}
	else {
		concrete_disk_io = std::make_unique<Direct_Disk_IO>(this->disk, this->info);
	}

	concrete_disk_io->Proceed_Task(task);
}

bool Disk_IO::Are_Old_Sectors_Needed(std::vector<std::uint64_t> sectors, std::uint64_t start, size_t how_many) {
	if (start != 0) {
		return true;
	}

	size_t sectors_count = sectors.size();
	std::uint64_t sector_size = this->info->Bytes_Per(Data_Block::SECTOR);
	std::uint64_t total_size = sector_size * sectors_count;

	if (total_size == how_many) {
		return false;
	}
	else {
		return true;
	}
}

Concrete_Disk_IO::Concrete_Disk_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->disk = disk;
	this->info = info;
}


void Direct_Disk_IO::Proceed_Task(std::shared_ptr<IO_Task> task) {
	std::uint64_t sector_count = task->data_blocks.size();

	if (task->type == Task_Type::WRITE) {
		this->disk->Write(sector_count, task->buffer, task->data_blocks[0]);
	}
	else {
		this->disk->Read(sector_count, task->buffer, task->data_blocks[0]);
	}
}

void Via_Buffer_Disk_IO::Proceed_Task(std::shared_ptr<IO_Task> task) {
	std::uint64_t sector_count = task->data_blocks.size();
	std::uint64_t sector_size = this->info->Bytes_Per(Data_Block::SECTOR);
	std::uint64_t total_size = sector_size * sector_count;

	std::uint8_t* data = new std::uint8_t[total_size];

	this->disk->Read(sector_count, static_cast<void*>(data), task->data_blocks[0]);
	
	if (task->type == Task_Type::WRITE) {
		memcpy(static_cast<void*>(data + task->starting_byte), task->buffer, task->how_many_bytes_to_operate);
		this->disk->Write(sector_count, static_cast<void*>(data), task->data_blocks[0]);
	}
	else {
		memcpy(task->buffer, static_cast<void*>(data + task->starting_byte), task->how_many_bytes_to_operate);
	}
	

	delete[] data;

}

