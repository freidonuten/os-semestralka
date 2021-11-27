#include "actions.h"

std::tuple<Open_Result, bool> try_delete(VFS& vfs, char* filename) {
	Delete_Result delete_result = actions::delete_file(vfs, filename);

	switch (delete_result) {
	case Delete_Result::OK:
		return { Open_Result::OK, true };
	case Delete_Result::FILE_NOT_EXISTING:
		return { Open_Result::OK, true };
	case Delete_Result::FILE_OPENED:
		return { Open_Result::ALREADY_OPENED, false};
	case Delete_Result::CANT_REMOVE:
		return { Open_Result::CANT_REMOVE_PREVIOUS, false};
	}

	return { Open_Result::UNKNOWN_ERROR, false };
}

std::tuple<std::uint16_t, Open_Result> actions::create_file(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	if (utils::Is_Valid_Filename(filename) == false) {
		return { 0, Open_Result::INVALID_FILENAME };
	}

	auto [try_delete_result, to_continue] = try_delete(vfs, filename);
	if (!to_continue) {
		return { 0, try_delete_result };
	}

	auto [cwd_path, file_path, cwd_dir] = actions::get_paths_and_directory(vfs, filename);
	auto element = vfs.Make_File(cwd_dir->Get_Fat_Directory(), filename, file_attrs);

	bool created = element->Create();
	if (!created) {
		return { 0, Open_Result::NO_MEMORY };
	}
	auto dir_entry = element->Generate_Dir_Entry();
	auto create_entry_result = (cwd_dir->Create_New_Entry(dir_entry));

	if (create_entry_result == Create_New_Entry_Result::ALREADY_EXISTS) {
		//strange error, we have removed the file on line 1 this function
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