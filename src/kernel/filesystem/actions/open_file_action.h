#pragma once

#include "action.h"

class Open_File_Action : public Action {
private:
	std::shared_ptr<VFS_Element> Open(std::shared_ptr<VFS_Element> parent, char filename[MAX_FILENAME_SIZE]);
	std::shared_ptr<VFS_Element> Create(std::shared_ptr<VFS_Element> parent, char filename[MAX_FILENAME_SIZE], std::uint16_t file_attributes);
public:
	using Action::Action;
	virtual void Run();
};