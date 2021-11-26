#include "actions.h"

Set_File_Attrs_Result actions::set_file_attrs(VFS& vfs, char* filename, std::uint8_t file_attrs) {
	auto [cwd_path, file_path, cwd_dir] = get_paths_and_directory(vfs, filename);

	bool exists = vfs.Get_Path_Handlers()->Check_If_Exists(file_path);
	if (exists) {
		return Set_File_Attrs_Result::FILE_OPENED;
	}

	auto [dir_entry, found] = cwd_dir->Read_Entry_By_Name(filename);
	if (!found) {
		return Set_File_Attrs_Result::FILE_NOT_EXISTING;
	}

	auto element = vfs.Make_File(cwd_dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);
	bool changed = element->Set_File_Attributes(file_attrs);
	if (!changed) {
		element->Close();
		return Set_File_Attrs_Result::CANT_CHANGE;
	}


	return Set_File_Attrs_Result::OK;
}