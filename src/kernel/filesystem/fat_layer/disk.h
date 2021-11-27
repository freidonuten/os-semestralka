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

class Hal_Disk : public IDisk {
private:
	int drive_id;

	
public:
	Hal_Disk(int drive_id);

	virtual void Write(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index);
	virtual void Read(std::uint64_t sector_count, void* sectors_buffer, std::uint64_t lba_index);
};