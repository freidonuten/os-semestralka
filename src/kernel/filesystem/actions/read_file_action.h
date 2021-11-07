#pragma once

#include "action.h"

class Read_File_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};