#pragma once

#include "action.h"
#include "file_descriptor.h"

class Close_Handle_Action : public Action {
public:
	using Action::Action;
	virtual void Run();
};