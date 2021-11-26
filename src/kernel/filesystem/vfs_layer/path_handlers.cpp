#include "path_handlers.h"

Path_Handlers::Path_Handlers(std::shared_ptr<Handler_Table> handler_table) {
	this->handler_table = handler_table;
}

std::tuple<std::uint16_t, bool> Path_Handlers::Add_Path_Element(std::shared_ptr<VFS_Element> element, std::shared_ptr<Path> path, int cwd_count, bool is_open) {
	if (this->Check_If_Exists(path)) {
		return { 0, false };
	}

	std::uint16_t result = Add_Handle(element, path, cwd_count, is_open);
	return { result, true };
}



std::tuple<std::shared_ptr<VFS_Element>, Handle_Close_Result> Path_Handlers::Close_Handle(std::uint16_t id) {
	auto it_int = this->map_id_path.find(id);

	if (it_int == this->map_id_path.end()) {
		//maybe it's only in handler table
		auto [element, found] = this->handler_table->Remove_Element(id);
		if (!found) {
			//it's not
			return { nullptr, Handle_Close_Result::NOT_EXISTS };
		}
		return { element, Handle_Close_Result::CLOSED };
	}

	std::string path = it_int->second;
	auto it_path = this->map_path_handle.find(path);

	//we will not check if it_path find the element
	//if not program is in inconsistent state, it's better to crash

	std::shared_ptr<Handle_Info> handle = it_path->second;
	handle->is_open = false;
	Try_Remove(handle);

}

std::tuple<std::uint16_t, Handle_Open_Result> Path_Handlers::Try_Open_Element(std::shared_ptr<Path> path) {
	auto it = this->map_path_handle.find(path->To_String());

	if (it == this->map_path_handle.end()) {
		return { 0, Handle_Open_Result::NOT_EXISTS };
	}

	if (it->second->is_open) {
		return { 0, Handle_Open_Result::ALREADY_OPENED };
	}

	it->second->is_open = true;
	return { it->second->id, Handle_Open_Result::RETURNED };
}

bool Path_Handlers::Check_If_Exists(std::shared_ptr<Path> path) {
	std::string new_path = path->To_String();

	return  !(this->map_path_handle.find(new_path) == this->map_path_handle.end());
}

std::tuple<std::shared_ptr<VFS_Element>, bool> Path_Handlers::Try_Set_CWD(std::shared_ptr<Path> path) {
	auto it = this->map_path_handle.find(path->To_String());

	if (it == this->map_path_handle.end()) {
		return { nullptr, false };
	}

	it->second->cwd_count++;
	
	auto [element, found] = this->handler_table->Get_Element(it->second->id);

	if (!found) {
		//INCONSISTENT STATE
		return { nullptr, false };
	}

	return { element, true };
}

void Path_Handlers::Unset_CWD(std::shared_ptr<Path> path) {
	auto it = this->map_path_handle.find(path->To_String());

	if (it == this->map_path_handle.end()) {
		return;
	}

	it->second->cwd_count--;
	Try_Remove(it->second);
}


void Path_Handlers::Try_Remove(std::shared_ptr<Handle_Info> handle) {
	if (handle->cwd_count <= 0 && handle->is_open == false) {
		this->map_path_handle.erase(handle->path->To_String());
		this->map_id_path.erase(handle->id);
		this->handler_table->Remove_Element(handle->id);	
	}
}

std::uint16_t Path_Handlers::Add_Handle(std::shared_ptr<VFS_Element> element, std::shared_ptr<Path> path, int cwd_count, bool is_open) {
	std::shared_ptr<Handle_Info> new_handle = std::make_shared<Handle_Info>();
	new_handle->cwd_count = cwd_count;
	new_handle->is_open = is_open;
	new_handle->id = this->handler_table->Create_Descriptor(element);
	new_handle->path = path;

	std::string string_path = path->To_String();
	
	this->map_path_handle.insert({string_path, new_handle});
	this->map_id_path.insert({ new_handle->id, string_path });

	return new_handle->id;
}