#include "actions.h"

Set_File_Attrs_Result set_file_attrs_using_parent(VFS& vfs, std::shared_ptr<Path> parent_path,
	std::shared_ptr<Path> file_path, std::shared_ptr<VFS_Directory> parent_dir, char* filename, std::uint8_t file_attrs) {

	bool exists = vfs.Get_Path_Handlers()->Check_If_Exists(file_path);
	if (exists) {
		return Set_File_Attrs_Result::FILE_OPENED;
	}

	auto [dir_entry, found] = parent_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return Set_File_Attrs_Result::FILE_NOT_EXISTING;
	}

	auto element = vfs.Make_File(parent_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);
	bool changed = element->Set_File_Attributes(file_attrs);
	if (!changed) {
		element->Close();
		return Set_File_Attrs_Result::CANT_CHANGE;
	}

	return Set_File_Attrs_Result::OK;
}

Set_File_Attrs_Result set_relative(VFS& vfs, char* path, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_relative(vfs, path);
	if (!is_valid) {
		return Set_File_Attrs_Result::FILE_NOT_EXISTING;
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return set_file_attrs_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}

Set_File_Attrs_Result set_absolute(VFS& vfs, char* path, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir, is_valid] = actions::get_paths_and_directory_absolute(vfs, path);
	if (!is_valid) {
		return Set_File_Attrs_Result::FILE_NOT_EXISTING;
	}

	char filename[12];
	file_path->Get_Filename(filename);

	return set_file_attrs_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}

Set_File_Attrs_Result set_filename(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	auto [parent_path, file_path, parent_dir] = actions::get_paths_and_directory_filename(vfs, filename);
	return set_file_attrs_using_parent(vfs, parent_path, file_path, parent_dir, filename, file_attrs);
}


Set_File_Attrs_Result actions::set_file_attrs(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	Filename_Type type = utils::Get_Filename_Type(filename);

	switch (type) {
	case Filename_Type::CURRENT:
		return Set_File_Attrs_Result::CANT_CHANGE;
	case Filename_Type::RELATIVE_PATH:
		return set_relative(vfs, filename, file_attrs);
	case Filename_Type::ABSOLUTE_PATH:
		return set_absolute(vfs, filename, file_attrs);
	case Filename_Type::FILENAME:
		return set_filename(vfs, filename, file_attrs);
	case Filename_Type::INVALID:
		return Set_File_Attrs_Result::FILE_NOT_EXISTING;
	}

	return Set_File_Attrs_Result::UNKNOWN_ERROR;
}