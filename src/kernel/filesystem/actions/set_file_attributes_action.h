#pragma once

#include "action.h"

class Set_File_Attributes_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};