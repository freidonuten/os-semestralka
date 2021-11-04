#include "global_structs.h"

#include "char_utils.h"

std::shared_ptr<IO_Task> IO_Task_Factory::Make_Task(
	std::vector<std::uint64_t> data_blocks, std::uint64_t starting_byte,
	size_t how_many_bytes_to_operate, void* buffer, Task_Type type) {

	std::shared_ptr<IO_Task> result = std::make_shared<IO_Task>();
	result->data_blocks = data_blocks;
	result->starting_byte = starting_byte;
	result->how_many_bytes_to_operate = how_many_bytes_to_operate;
	result->buffer = buffer;
	result->type = type;

	return result;
}

Fat_Dir_Entry Fat_Dir_Entry_Factory::Create( std::uint16_t file_attributes, char file_name[12],
	std::uint16_t file_start, std::uint64_t file_size) {

	Fat_Dir_Entry result;
	result.file_attributes = file_attributes;
	result.file_size = file_size;
	result.file_start = file_start;

	Char_Utils::Copy_Array(result.file_name, file_name, 12);

	return result;
}