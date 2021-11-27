#pragma once


#include "..\api\hal.h"
#include "..\api\api.h"
#include "Task_Manager.h"

extern Task_Manager task_manager;

void Set_Error(const kiv_os::NOS_Error error, kiv_hal::TRegisters &regs);
void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context);
