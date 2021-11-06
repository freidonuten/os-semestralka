#pragma once

#include "action.h"

class Set_Working_Dir_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};