#pragma once
#include "command.h"
#include "command_executor.h"
#include "../api/api.h"
#include <vector>
#include <map>
#include <memory>

class CommandExecutor {
public:
	void Execute_Command(std::vector<Command> commands, const kiv_os::THandle &stdin_handle, const kiv_os::THandle &stdout_handle);
};