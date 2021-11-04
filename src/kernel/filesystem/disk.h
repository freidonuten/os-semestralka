#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class IDisk {
private:

public:
	virtual void Write(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) = 0;
	virtual void Read(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index) = 0;
};

class Dummy_Disk_Operator {
protected:
	std::shared_ptr<std::vector<unsigned char>> data;

public:
	virtual void Operate(std::uint64_t position_to_operate, unsigned char* sectors_buffer_element) = 0;
};

class Dummy_Disk : public IDisk {
private:
	std::shared_ptr<std::vector<unsigned char>> data;
	std::uint64_t sector_size;

	void Operate(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index, std::unique_ptr<Dummy_Disk_Operator> disk_operator);
public:
	Dummy_Disk(std::uint64_t disk_size, std::uint64_t sector_size);
	virtual void Write(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index);
	virtual void Read(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index);
};



class Dummy_Disk_Writer : public Dummy_Disk_Operator {
public:
	Dummy_Disk_Writer(std::shared_ptr<std::vector<unsigned char>> data);
	virtual void Operate(std::uint64_t position_to_operate, unsigned char* sectors_buffer_element);
};

class Dummy_Disk_Reader : public Dummy_Disk_Operator {
public:
	Dummy_Disk_Reader(std::shared_ptr<std::vector<unsigned char>> data);
	virtual void Operate(std::uint64_t position_to_operate, unsigned char* sectors_buffer_element);
};