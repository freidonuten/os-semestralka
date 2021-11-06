#pragma once

#include "action.h"
#include "path.h"

class Get_File_Attributes_Action : public Action {
private:
	std::shared_ptr<Path_Converter> path_converter;
	std::shared_ptr<VFS_Element> current_directory;
public:
	using Action::Action;
	virtual void Run();
};