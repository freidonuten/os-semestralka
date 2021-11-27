#pragma once

#include "cwd.h"
#include "../vfs_layer/vfs_directory.h"

#include <tuple>
#include <map>

//will be replaced by process methods

class Dummy_CWD_Holder {
private:
	using CWD_pair = std::pair<std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>>;
	std::map<kiv_os::THandle, CWD_pair> cwd_mapping;

public:
	Dummy_CWD_Holder(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory>);
	void Inherit(kiv_os::THandle ppid, kiv_os::THandle cpid);
	void Set_Value(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory> directory);
	std::tuple<std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> Get_Value();
};