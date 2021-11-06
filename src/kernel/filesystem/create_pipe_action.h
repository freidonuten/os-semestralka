#pragma once

#include "action.h"

class Create_Pipe_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};
