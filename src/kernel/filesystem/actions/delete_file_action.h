#pragma once

#include "action.h"

class Delete_File_Action : public Action {
public:
	using Action::Action;
	virtual void Run();
};