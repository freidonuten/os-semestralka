#include "disk.h"


Dummy_Disk::Dummy_Disk(std::uint64_t Disk_size, std::uint64_t sector_size) {
	std::vector<unsigned char> temp_vector(Disk_size);
	this->data = std::make_shared<std::vector<unsigned char>>(temp_vector);
	this->sector_size = sector_size;
}

void Dummy_Disk::Write(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) {
	std::unique_ptr<Dummy_Disk_Operator> writer = std::make_unique<Dummy_Disk_Writer>(this->data);
	Operate(sector_count, sectors_buffer, lba_index, std::move(writer));
}

void Dummy_Disk::Read(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) {
	std::unique_ptr<Dummy_Disk_Operator> reader = std::make_unique<Dummy_Disk_Reader>(this->data);
	Operate(sector_count, sectors_buffer, lba_index, std::move(reader));
}

void Dummy_Disk::Operate(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index, std::unique_ptr<Dummy_Disk_Operator> Disk_operator) {
	std::uint64_t starting_element = lba_index * this->sector_size;
	std::uint64_t bytes_to_operate = sector_count * this->sector_size;
	for (std::int64_t i = 0; i < bytes_to_operate; i++) {
		std::uint64_t position_to_operate = starting_element + i;
		unsigned char* buffer_element = static_cast<unsigned char*>(sectors_buffer) + i;
		Disk_operator->Operate(position_to_operate, buffer_element);
	}
}


Dummy_Disk_Writer::Dummy_Disk_Writer(std::shared_ptr<std::vector<unsigned char>> data) {
	this->data = data;
}

Dummy_Disk_Reader::Dummy_Disk_Reader(std::shared_ptr<std::vector<unsigned char>> data) {
	this->data = data;
}

void Dummy_Disk_Writer::Operate(std::uint64_t position_to_operate, unsigned char* sectors_buffer_element) {
	unsigned char element = *sectors_buffer_element;
	(*this->data)[position_to_operate] = element;
}

void Dummy_Disk_Reader::Operate(std::uint64_t position_to_operate, unsigned char* sectors_buffer_element) {
	*sectors_buffer_element = (*this->data)[position_to_operate];
}