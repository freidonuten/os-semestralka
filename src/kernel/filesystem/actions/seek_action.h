#pragma once

#include "action.h"

class Seek_Action : public Action {
private:
public:
	using Action::Action;
	virtual void Run();
};