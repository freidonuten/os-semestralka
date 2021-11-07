#pragma once

#include "../../../api/hal.h"
#include "../../../api/api.h"

#include "../utils/global_constants.h"
#include "../vfs_layer/path.h"
#include "../vfs.h"


#include <memory>


class Action {
protected:
	kiv_hal::TRegisters *registers;
	VFS *vfs;
public:
	Action(kiv_hal::TRegisters &registers, VFS *vfs);
	virtual void Run() = 0;
};

