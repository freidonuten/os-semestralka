#include "actions.h"

std::tuple<std::shared_ptr<Path>, Set_CWD_Result, bool> Get_New_CWD(std::shared_ptr<Path> old_cwd, char* path) {
	std::shared_ptr<Path> new_cwd;

	Path_Type type = utils::Get_Path_Type(path);
	if (type == Path_Type::ABSOLUTE_PATH) {
		new_cwd = std::make_shared<Path>(path);
		return{ new_cwd, Set_CWD_Result::OK, true };
	}
	else if (type == Path_Type::RELATIVE_PATH) {
		new_cwd = std::make_shared<Path>(*old_cwd); //copy
		new_cwd->Append(path);
		return{ new_cwd, Set_CWD_Result::OK, true };
	}
	else {
		return { nullptr, Set_CWD_Result::INVALID_PATH, false };
	}

	return { nullptr, Set_CWD_Result::UNKNOWN_ERROR, false };
}

std::tuple<std::shared_ptr<VFS_Directory>, Get_CWD_From_Handle_Result> Get_From_Handle(VFS& vfs, std::shared_ptr<Path> path) {
	auto[element, found] = vfs.Get_Path_Handlers()->Try_Set_CWD(path);

	if (!found) {
		return { nullptr, Get_CWD_From_Handle_Result::NOT_FOUND };
	}

	auto directory = std::dynamic_pointer_cast<VFS_Directory>(element);
	if (directory == NULL) {
		return { nullptr, Get_CWD_From_Handle_Result::NOT_A_DIRECTORY };
	}

	return { directory, Get_CWD_From_Handle_Result::VALID_DIRECTORY };
}

std::tuple<std::shared_ptr<VFS_Directory>, Set_CWD_Result, bool> Try_Open_Directory(VFS& vfs, std::shared_ptr<Path> new_cwd) {
	auto [new_dir, error] = vfs.Open_Directory(new_cwd);

	switch (error) {
	case Open_Directory_Error::NOT_A_DIRECTORY:
		return { nullptr, Set_CWD_Result::NOT_A_DIRECTORY, false };
	case Open_Directory_Error::PATH_NOT_EXISTING:
		return { nullptr, Set_CWD_Result::PATH_NOT_FOUND, false };
	case Open_Directory_Error::OK:
		return { new_dir, Set_CWD_Result::OK, true };
	}

	return { nullptr, Set_CWD_Result::UNKNOWN_ERROR, false };
}



Set_CWD_Result actions::set_cwd(VFS& vfs, char* path) {
	std::shared_ptr<VFS_Directory> new_cwd_dir;
	auto [old_cwd_path, old_cwd_dir] = vfs.Get_CWD();

	auto [new_cwd_path, error_result, to_continue] = Get_New_CWD(old_cwd_path, path);
	if (!to_continue) {
		return error_result;
	}

	auto [handle_directory, op_result] = Get_From_Handle(vfs, new_cwd_path);
	if (op_result == Get_CWD_From_Handle_Result::NOT_A_DIRECTORY) {
		return Set_CWD_Result::NOT_A_DIRECTORY;
	}
	else if (op_result == Get_CWD_From_Handle_Result::VALID_DIRECTORY) {
		new_cwd_dir = handle_directory;
	}
	else if (op_result == Get_CWD_From_Handle_Result::NOT_FOUND) {
		auto [opened_directory, open_error_result, to_continue_nested] = Try_Open_Directory(vfs, new_cwd_path);
		if (to_continue_nested) {
			new_cwd_dir = opened_directory;
			vfs.Get_Path_Handlers()->Add_Path_Element(new_cwd_dir, new_cwd_path, 1, false);
		}
		else {
			return open_error_result;
		}
	}
	else {
		return Set_CWD_Result::UNKNOWN_ERROR;
	}

	vfs.Set_CWD(new_cwd_path, new_cwd_dir);
	auto [temp1, temp2] = vfs.Get_CWD();
	vfs.Get_Path_Handlers()->Unset_CWD(old_cwd_path);
	return Set_CWD_Result::OK;
}