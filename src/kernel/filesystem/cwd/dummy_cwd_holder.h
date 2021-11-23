#pragma once

#include "cwd.h"
#include "../vfs2/vfs_directory.h"

#include <tuple>

//will be replaced by process methods

class Dummy_CWD_Holder {
private:
	std::tuple<std::shared_ptr<CWD>, std::shared_ptr<VFS_Directory2>> value;
public:
	Dummy_CWD_Holder(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory2>);
	void Set_Value(std::shared_ptr<CWD> cwd, std::shared_ptr<VFS_Directory2> directory);
	std::tuple<std::shared_ptr<CWD>, std::shared_ptr<VFS_Directory2>> Get_Value();
};