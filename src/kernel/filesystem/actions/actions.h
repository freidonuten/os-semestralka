#pragma once

#include "../utils/global_structs.h"
#include "../utils/char_utils.h"
#include "../cwd/cwd.h"
#include "../vfs_layer/vfs_directory.h"
#include "../vfs_layer/vfs.h"


#include <memory>

namespace actions {
	std::tuple<std::uint16_t, Open_Result> open_file(VFS& vfs, char* filename);
	std::tuple<std::uint16_t, Open_Result> create_file(VFS& vfs, char* filename, std::uint8_t file_attrs);
	Delete_Result delete_file(VFS& vfs, char* filename);
	Set_File_Attrs_Result set_file_attrs(VFS& vfs, char* filename, std::uint8_t file_attrs);
	Set_CWD_Result set_cwd(VFS& vfs, char* path);
	std::tuple<std::shared_ptr<Path>, std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> get_paths_and_directory(VFS& vfs, char* filename);
};

