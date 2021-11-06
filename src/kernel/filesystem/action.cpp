#include "action.h"

Action::Action(std::shared_ptr<kiv_hal::TRegisters> registers, std::shared_ptr<VFS> vfs) {
	this->registers = registers;
	this->vfs = vfs;
}

