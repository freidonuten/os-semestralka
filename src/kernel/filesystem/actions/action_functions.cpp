#include "actions.h"

std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> actions::get_paths_and_directory(VFS& vfs,  char* filename) {
	auto [cwd_path, cwd_dir] = vfs.Get_CWD();
	auto file_path = std::make_shared<Path>(*cwd_path); //copy
	file_path->Append(filename);

	return{ cwd_path, file_path, cwd_dir };
}