#pragma once

#include "action.h"

class Get_Working_Dir_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};