#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../cwd/cwd.h"

enum class Path_Type : std::uint8_t {
	ABSOLUTE_PATH,
	RELATIVE_PATH,
	INVALID
};

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
	std::uint8_t file_attributes;
	char file_name[12];
	std::uint16_t file_start;
	std::uint64_t file_size;
};

class Fat_Dir_Entry_Factory {
public:
	static Fat_Dir_Entry Create( std::uint8_t file_attributes, char file_name[12],
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
};

enum class Seek_Result : std::uint8_t {
	NO_ERROR_POSITION_RETURNED,
	NO_ERROR_POSITION_NOT_RETURNED,
	ERROR_SETTING_SIZE,
	ERROR_INVALID_PARAMETERS
};

struct Handle_Info {
	int cwd_count;
	bool is_open;
	std::uint16_t id;
	std::shared_ptr<Path> path;
};

enum class Handle_Open_Result {
	RETURNED,
	NOT_EXISTS,
	ALREADY_OPENED
};

enum class Handle_Close_Result {
	CLOSED,
	NOT_EXISTS,
	UNKNOWN_ERROR
};

enum class Open_Result {
	OK,
	INVALID_FILENAME,
	FILE_NOT_FOUND,
	ALREADY_OPENED,
	CANT_REMOVE_PREVIOUS,
	NO_MEMORY,
	UNKNOWN_ERROR
};

enum class Delete_Result {
	OK,
	FILE_OPENED,
	FILE_NOT_EXISTING,
	CANT_REMOVE,
	UNKNOWN_ERROR
};

enum class Set_File_Attrs_Result {
	OK,
	FILE_OPENED,
	FILE_NOT_EXISTING,
	CANT_CHANGE,
	UNKNOWN_ERROR
};

enum class Set_CWD_Result {
	OK,
	INVALID_PATH,
	PATH_NOT_FOUND,
	NOT_A_DIRECTORY,
	UNKNOWN_ERROR
};

enum class Get_CWD_From_Handle_Result {
	VALID_DIRECTORY,
	NOT_A_DIRECTORY,
	NOT_FOUND
};

enum class Create_New_Entry_Result {
	OK,
	NO_MEMORY,
	ALREADY_EXISTS
};

enum class Filename_Type {
	CURRENT,
	FILENAME,
	RELATIVE_PATH,
	ABSOLUTE_PATH,
	INVALID
};

