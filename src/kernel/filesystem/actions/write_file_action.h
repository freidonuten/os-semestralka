#pragma once

#include "action.h"

class Write_File_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};