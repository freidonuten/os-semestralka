#pragma once

#include "../../api/hal.h"
#include "../../api/api.h"

#include "vfs.h"

#include <memory>

class Action {
protected:
	std::shared_ptr<kiv_hal::TRegisters> registers;
	std::shared_ptr<VFS> vfs;
public:
	Action(std::shared_ptr<kiv_hal::TRegisters> registers, std::shared_ptr<VFS> vfs);
	virtual void Run() = 0;
};

