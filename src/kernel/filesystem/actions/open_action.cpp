#include "actions.h"
#include "../sysfs_layer/proc_file.h"

std::tuple<std::uint16_t, Open_Result, bool> try_open_handler(VFS& vfs, std::shared_ptr<Path> file_path) {
	auto [returned_handler_id, try_open_result] = vfs.Get_Path_Handlers()->Try_Open_Element(file_path);

	switch (try_open_result) {
	case Handle_Open_Result::ALREADY_OPENED:
		return { 0, Open_Result::ALREADY_OPENED, false };
	case Handle_Open_Result::RETURNED:
		return { returned_handler_id, Open_Result::OK, false };
	case Handle_Open_Result::NOT_EXISTS:
		return { 0, Open_Result::OK, true };
	}

	return { 0, Open_Result::UNKNOWN_ERROR, false };
}

std::tuple<std::uint16_t, Open_Result> actions::open_file(VFS& vfs, char* filename) {
	if (utils::Is_Valid_Filename(filename) == false) {
		return { 0, Open_Result::INVALID_FILENAME };
	}

	auto [cwd_path, file_path, cwd_dir] = get_paths_and_directory(vfs, filename);

	if (file_path->To_String() == "/proc") {
		auto desc_table = vfs.Get_Handler_Table();
		auto proc_file = proc::Factory();

		const auto descriptor = desc_table->Create_Descriptor(proc_file);

		return { descriptor, Open_Result::OK };
	}

	auto [returned_handler_id, try_open_result, to_continue] = try_open_handler(vfs, file_path);
	if (!to_continue) {
		return { returned_handler_id, try_open_result };
	}

	auto [dir_entry, found] = cwd_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	auto element = vfs.Make_File(cwd_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);

	auto [opened_handler_id, already_inserted] = vfs.Get_Path_Handlers()->Add_Path_Element(element, file_path, 0, true);
	if (already_inserted) {
		return { 0, Open_Result::UNKNOWN_ERROR };
	}

	return { opened_handler_id, Open_Result::OK };
}