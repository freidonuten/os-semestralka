#include "actions.h"

Delete_Result actions::delete_file(VFS& vfs, char* filename) {
	auto [cwd_path, file_path, cwd_dir] = get_paths_and_directory(vfs, filename);

	bool exists = vfs.Get_Path_Handlers()->Check_If_Exists(file_path);
	if (exists) {
		return Delete_Result::FILE_OPENED;
	}

	auto [dir_entry, found] = cwd_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return Delete_Result::FILE_NOT_EXISTING;
	}

	auto element = vfs.Make_File(cwd_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);
	bool deleted = element->Remove();
	if (!deleted) {
		element->Close();
		return Delete_Result::CANT_REMOVE;
	}

	bool removed_from_dir = cwd_dir->Remove_Entry(filename);
	if (!removed_from_dir) {
		return Delete_Result::UNKNOWN_ERROR;
	}

	return Delete_Result::OK;
}