#include "action.h"

Action::Action(kiv_hal::TRegisters &registers, VFS* vfs) {
	this->registers = &registers;
	this->vfs = vfs;
}

