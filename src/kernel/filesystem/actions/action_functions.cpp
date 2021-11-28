#include "actions.h"

std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> actions::get_paths_and_directory_filename(VFS& vfs, char* filename) {
	auto [cwd_path, cwd_dir] = vfs.Get_CWD();
	auto file_path = std::make_shared<Path>(*cwd_path); //copy
	file_path->Append(filename);

	return{ cwd_path, file_path, cwd_dir };
}


std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>, bool> get_paths_from_cwd(VFS& vfs, std::shared_ptr<Path> file_path) {
	auto parent_path = std::make_shared<Path>(*file_path); //copy
	parent_path->Append(".."); //get parent

	auto [parent_dir, error] = vfs.Open_Directory(parent_path);

	switch (error) {
	case Open_Directory_Error::NOT_A_DIRECTORY:
	case Open_Directory_Error::PATH_NOT_EXISTING:
		return { nullptr, nullptr, nullptr, false };
	case Open_Directory_Error::OK:
		return { parent_path, file_path, parent_dir, true };
	}

	return { nullptr, nullptr, nullptr, false };
}

std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>, bool> actions::get_paths_and_directory_relative(VFS& vfs, char* relative_path) {
	auto [cwd_path, cwd_dir] = vfs.Get_CWD();
	auto file_path = std::make_shared<Path>(*cwd_path); //copy
	file_path->Append(relative_path);

	return get_paths_from_cwd(vfs, file_path);	
}

std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>, bool> actions::get_paths_and_directory_absolute(VFS& vfs, char* absolute_path) {
	auto file_path = std::make_shared<Path>(absolute_path);
	return get_paths_from_cwd(vfs, file_path);
}