#include "actions.h"

std::tuple<Open_Result, bool> try_delete(VFS& vfs, char* filename) {
	Delete_Result delete_result = actions::delete_file(vfs, filename);

	switch (delete_result) {
	case Delete_Result::OK:
		return { Open_Result::OK, true };
	case Delete_Result::FILE_NOT_EXISTING:
		return { Open_Result::OK, true };
	case Delete_Result::FILE_OPENED:
		return { Open_Result::ALREADY_OPENED, false };
	case Delete_Result::CANT_REMOVE:
		return { Open_Result::CANT_REMOVE_PREVIOUS, false };
	}

	return { Open_Result::UNKNOWN_ERROR, false };
}

std::tuple<std::uint16_t, Open_Result> create_file_using_parent(VFS& vfs, std::shared_ptr<Path> parent_path,
	std::shared_ptr<Path> file_path, std::shared_ptr<VFS_Directory> parent_dir, char* filename, std::uint8_t file_attrs) {

	//root filtered out via delete

	auto element = vfs.Make_File(parent_dir->Get_Fat_Directory(), filename, file_attrs);
	if (!element) {
		return { 0, Open_Result::INVALID_FILE_TYPE };
	}

	bool created = element->Create();
	if (!created) {
		return { 0, Open_Result::NO_MEMORY };
	}
	auto dir_entry = element->Generate_Dir_Entry();
	auto create_entry_result = (parent_dir->Create_New_Entry(dir_entry));

	if (create_entry_result == Create_New_Entry_Result::ALREADY_EXISTS) {
		//strange error, we have already removed the file
		element->Remove();
		return { 0, Open_Result::UNKNOWN_ERROR };
	}

	if (create_entry_result == Create_New_Entry_Result::NO_MEMORY) {
		element->Remove();
		return { 0, Open_Result::NO_MEMORY };
	}

	auto [created_handler_id, is_ok] = vfs.Get_Path_Handlers()->Add_Path_Element(element, file_path, 0, true);
	if (!is_ok) {
		return { 0, Open_Result::UNKNOWN_ERROR };
	}

	return { created_handler_id, Open_Result::OK };
}

std::tuple<std::uint16_t, Open_Result> create_relative(VFS& vfs, char* path, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_relative(vfs, path);
	if (!is_valid) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return create_file_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}

std::tuple<std::uint16_t, Open_Result> create_absolute(VFS& vfs, char* path, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_absolute(vfs, path);
	if (!is_valid) {
		return { 0, Open_Result::FILE_NOT_FOUND };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return create_file_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}

std::tuple<std::uint16_t, Open_Result> create_filename(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir] = actions::get_paths_and_directory_filename(vfs, filename);
	return create_file_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}

std::tuple<std::uint16_t, Open_Result> actions::create_file(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	auto [try_delete_result, to_continue] = try_delete(vfs, filename);
	if (!to_continue) {
		return { 0, try_delete_result };
	}

	Filename_Type type = utils::Get_Filename_Type(filename);

	switch (type) {
	case Filename_Type::CURRENT:
		return { 0, Open_Result::CANT_REMOVE_PREVIOUS };
	case Filename_Type::ABSOLUTE_PATH:
		return create_absolute(vfs, filename, file_attrs);
	case Filename_Type::RELATIVE_PATH:
		return create_relative(vfs, filename, file_attrs);
	case Filename_Type::FILENAME:
		return create_filename(vfs, filename, file_attrs);
	case Filename_Type::INVALID:
		return { 0, Open_Result::INVALID_FILENAME };
	}

	return { 0, Open_Result::UNKNOWN_ERROR };
}