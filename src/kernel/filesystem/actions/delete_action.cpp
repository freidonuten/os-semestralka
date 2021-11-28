#include "actions.h"

Delete_Result delete_file_using_parent(VFS& vfs, std::shared_ptr<Path> parent_path,
	std::shared_ptr<Path> file_path, std::shared_ptr<VFS_Directory> parent_dir, char* filename) {

	if (file_path->To_String() == "/") {
		return Delete_Result::CANT_REMOVE;
	}

	bool exists = vfs.Get_Path_Handlers()->Check_If_Exists(file_path);
	if (exists) {
		return Delete_Result::FILE_OPENED;
	}

	auto [dir_entry, found] = parent_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return Delete_Result::FILE_NOT_EXISTING;
	}

	auto element = vfs.Make_File(parent_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);
	bool deleted = element->Remove();
	if (!deleted) {
		element->Close();
		return Delete_Result::CANT_REMOVE;
	}

	bool removed_from_dir = parent_dir->Remove_Entry(filename);
	if (!removed_from_dir) {
		return Delete_Result::UNKNOWN_ERROR;
	}

	return Delete_Result::OK;
}

Delete_Result delete_absolute(VFS& vfs, char* path) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_absolute(vfs, path);

	if (!is_valid) {
		return { Delete_Result::FILE_NOT_EXISTING };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return delete_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}

Delete_Result delete_relative(VFS& vfs, char* path) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_relative(vfs, path);

	if (!is_valid) {
		return { Delete_Result::FILE_NOT_EXISTING };
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return delete_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}

Delete_Result delete_filename(VFS& vfs, char* filename) {
	auto [parent_path, file_path, parent_dir] = actions::get_paths_and_directory_filename(vfs, filename);
	return delete_file_using_parent(vfs, parent_path, file_path, parent_dir, filename);
}

Delete_Result actions::delete_file(VFS& vfs, char* filename) {
 	Filename_Type type = utils::Get_Filename_Type(filename);
	switch (type) {
	case Filename_Type::CURRENT:
		return Delete_Result::CANT_REMOVE;
	case Filename_Type::ABSOLUTE_PATH:
		return delete_absolute(vfs, filename);
	case Filename_Type::RELATIVE_PATH:
		return delete_relative(vfs, filename);
	case Filename_Type::FILENAME:
		return delete_filename(vfs, filename);
	case Filename_Type::INVALID:
		return Delete_Result::FILE_NOT_EXISTING;
	}

	return Delete_Result::UNKNOWN_ERROR;

}