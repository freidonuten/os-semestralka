#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include "handler_table.h"
#include "../vfs_layer/vfs_element.h"
#include "../utils/global_structs.h"
#include "../cwd/cwd.h"

class Path_Handlers {
private:
	std::shared_ptr<Handler_Table> handler_table;

	std::map<std::uint16_t, std::string> map_id_path;
	std::map<std::string, std::shared_ptr<Handle_Info>> map_path_handle;
	
	void Try_Remove(std::shared_ptr<Handle_Info> handle);
	std::uint16_t Add_Handle(std::shared_ptr<VFS_Element> element, std::shared_ptr<Path> path, int cwd_count, bool is_open);

public:

	Path_Handlers(std::shared_ptr<Handler_Table> handler_table);

	std::tuple<std::uint16_t, bool> Add_Path_Element(std::shared_ptr<VFS_Element> element, std::shared_ptr<Path> path, int cwd_count, bool is_open);
	std::tuple<std::shared_ptr<VFS_Element>, Handle_Close_Result> Close_Handle(std::uint16_t id);

	std::tuple<std::uint16_t, Handle_Open_Result> Try_Open_Element(std::shared_ptr<Path> path);
	bool Check_If_Exists(std::shared_ptr<Path> path);
	std::tuple<std::shared_ptr<VFS_Element>, bool> Try_Set_CWD(std::shared_ptr<Path> path);
	void Unset_CWD(std::shared_ptr<Path> path);

};