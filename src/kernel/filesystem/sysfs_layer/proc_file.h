#pragma once

#include "../vfs_layer/vfs_element.h"
#include "../../Task_Manager.h"

namespace proc {

	// Allow iterating process control block as a string in a line-by-line manner
	class File : public VFS_Element {
	private:
		Task_Manager::Process_Table::const_iterator begin;
		Task_Manager::Process_Table::const_iterator end;
		Task_Manager::Process_Table::const_iterator index;

	public:
		File() = delete;
		File(const Task_Manager& tm);

		// reads single PCB entry
		std::uint64_t Read(size_t how_many_bytes, void* buffer) override;
	};

	std::shared_ptr<File> Factory();
}
