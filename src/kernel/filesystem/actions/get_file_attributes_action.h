#pragma once

#include "action.h"

class Get_File_Attributes_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};