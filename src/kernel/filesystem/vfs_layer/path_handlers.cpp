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
		const auto element = this->handler_table->Remove_Element(id);
		if (!element) {
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
	const auto element = Try_Remove(handle);
	if (!element) {
		return { nullptr, Handle_Close_Result::UNKNOWN_ERROR };
	}

	return { element, Handle_Close_Result::CLOSED };

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
	
	const auto element = this->handler_table->Get_Element(it->second->id);

	if (!element) {
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


std::shared_ptr<VFS_Element> Path_Handlers::Try_Remove(std::shared_ptr<Handle_Info> handle) {
	if (handle->cwd_count <= 0 && !handle->is_open) {
		this->map_path_handle.erase(handle->path->To_String());
		this->map_id_path.erase(handle->id);
		return this->handler_table->Remove_Element(handle->id);	
	} else {
		return this->handler_table->Get_Element(handle->id);
	}
}

std::uint16_t Path_Handlers::Add_Handle(std::shared_ptr<VFS_Element> element, std::shared_ptr<Path> path, int cwd_count, bool is_open) {
	const auto new_id = handler_table->Create_Descriptor(element);
	const auto new_handle = std::make_shared<Handle_Info>(Handle_Info{ cwd_count, is_open, new_id, path });
	const auto string_path = path->To_String();
	
	this->map_path_handle.insert({ string_path, new_handle });
	this->map_id_path.insert({ new_id, string_path });

	return new_id;
}