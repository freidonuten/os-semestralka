#pragma once

#include "action.h"

class Close_Handle_Action : public Action {
public:
	using Action::Action;
	virtual void Run();
};