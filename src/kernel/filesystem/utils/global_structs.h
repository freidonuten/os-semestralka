#pragma once

#include <cstdint>
#include <memory>
#include <vector>


enum class Task_Type : std::uint8_t {
	READ,
	WRITE
};

enum class VFS_Fat_Element_Type : std::uint8_t {
	DIRECTORY,
	FILE,
	ROFILE,
	INVALID
};

enum class Open_Directory_Error : std::uint8_t {
	OK,
	PATH_NOT_EXISTING,
	NOT_A_DIRECTORY
};

struct IO_Task {
	std::vector<std::uint64_t> data_blocks; //sectors or clusters depending on context
	std::uint64_t starting_byte;
	size_t how_many_bytes_to_operate;
	void* buffer;
	Task_Type type;
};


class IO_Task_Factory {
public:
	static std::shared_ptr<IO_Task> Make_Task(
		std::vector<std::uint64_t> data_blocks, std::uint64_t starting_byte,
		size_t how_many_bytes_to_operate, void* buffer, Task_Type type);
};





struct Fat_Dir_Entry {
	std::uint16_t file_attributes;
	char file_name[12];
	std::uint16_t file_start;
	std::uint64_t file_size;
};

class Fat_Dir_Entry_Factory {
public:
	static Fat_Dir_Entry Create( std::uint16_t file_attributes, char file_name[12],
		std::uint16_t file_start, std::uint64_t file_size);
};


enum class NFile_Attributes : std::uint8_t {
	Read_Only = 0x01,
	Hidden = 0x02,
	System_File = 0x04,
	Volume_ID = 0x08,
	Fat_Directory = 0x10,
	Archive = 0x20
};

struct TDir_Entry {
	uint16_t file_attributes;			//viz konstanty NFile_Attributes
	char file_name[8 + 1 + 3];	//8.3 FAT
};

namespace global_structs {
	Fat_Dir_Entry Create_Empty_Fat_Dir_Entry();
}

enum class Seek_Result : std::uint8_t {
	NO_ERROR_POSITION_RETURNED,
	NO_ERROR_POSITION_NOT_RETURNED,
	ERROR_SETTING_SIZE,
	ERROR_INVALID_PARAMETERS
};