#pragma once

#include "cwd.h"
#include "../vfs2/vfs_directory.h"

#include <tuple>

//will be replaced by process methods

class Dummy_CWD_Holder {
private:
	std::tuple<CWD*, VFS_Directory2*> value;
public:
	Dummy_CWD_Holder(std::tuple<CWD*, VFS_Directory2*> value);
	void Set_Value(std::tuple<CWD*, VFS_Directory2*> value);
	std::tuple<CWD*, VFS_Directory2*> Get_Value();
};