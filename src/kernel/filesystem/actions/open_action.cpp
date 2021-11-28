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

std::tuple<std::uint16_t, Open_Result> open_current(VFS& vfs) {
	auto [cwd_path, cwd_dir] = vfs.Get_CWD();
	auto [id, result, to_continue] = try_open_handler(vfs, cwd_path);

	if (!to_continue) {
		return { id, result };
	}

	return { 0, Open_Result::UNKNOWN_ERROR };
}

std::tuple<std::uint16_t, Open_Result> open_root(VFS& vfs, std::shared_ptr<Path> path) {
	auto [id, result, to_continue] = try_open_handler(vfs, path);

	if (!to_continue) {
		return { id, result };
	}

	std::shared_ptr<VFS_Directory> root_dir = vfs.Get_Root();
	auto [opened_handler_id, already_inserted] = vfs.Get_Path_Handlers()->Add_Path_Element(root_dir, path, 0, true);
	if (already_inserted) {
		return { 0, Open_Result::UNKNOWN_ERROR };
	}
	
	return { opened_handler_id, Open_Result::OK };
}

std::tuple<std::uint16_t, Open_Result> open_proc(VFS& vfs) {
	auto desc_table = vfs.Get_Handler_Table();
	auto proc_file = proc::Factory();

	const auto descriptor = desc_table->Create_Descriptor(proc_file);

	return { descriptor, Open_Result::OK };
}


std::tuple<std::uint16_t, Open_Result> open_file_using_parent(VFS& vfs, std::shared_ptr<Path> parent_path, 
	std::shared_ptr<Path> file_path, std::shared_ptr<VFS_Directory> parent_dir, char* filename) {

	if (file_path->To_String() == "/") {
		return open_root(vfs, file_path);
	}

	if (file_path->To_String() == "/proc") {
		return open_proc(vfs);
	}

	auto [returned_handler_id, try_open_result, to_continue] = try_open_handler(vfs, file_path);
	if (!to_continue) {
		return { returned_handler_id, try_open_result };
	}

	auto [dir_entry, found] = parent_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	auto element = vfs.Make_File(parent_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);

	auto [opened_handler_id, already_inserted] = vfs.Get_Path_Handlers()->Add_Path_Element(element, file_path, 0, true);
	if (already_inserted) {
		return { 0, Open_Result::UNKNOWN_ERROR };
	}

	return { opened_handler_id, Open_Result::OK };
}

std::tuple<std::uint16_t, Open_Result> open_relative(VFS& vfs, char* path) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_relative(vfs, path);
	if (!is_valid) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return open_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}

std::tuple<std::uint16_t, Open_Result> open_absolute(VFS& vfs, char* path) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_absolute(vfs, path);
	if (!is_valid) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return open_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}

std::tuple<std::uint16_t, Open_Result> open_filename(VFS& vfs, char* filename) {
	auto [parent_path, file_path, parent_dir] = actions::get_paths_and_directory_filename(vfs, filename);
	return open_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}


std::tuple<std::uint16_t, Open_Result> actions::open_file(VFS& vfs, char* filename) {
	Filename_Type type = utils::Get_Filename_Type(filename);
	switch (type) {
	case Filename_Type::CURRENT:
		return open_current(vfs);
	case Filename_Type::FILENAME:
		return open_filename(vfs, filename);
	case Filename_Type::ABSOLUTE_PATH:
		return open_absolute(vfs, filename);
	case Filename_Type::RELATIVE_PATH:
		return open_relative(vfs, filename);
	case Filename_Type::INVALID:
		return { 0, Open_Result::INVALID_FILENAME };
	}

	return { 0, Open_Result::UNKNOWN_ERROR };
}