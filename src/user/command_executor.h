#pragma once
#include "command.h"
#include "command_executor.h"
#include "../api/api.h"
#include <vector>
#include <map>
#include <memory>

class CommandExecutor {
public:
	void executeCommand(std::vector<Command> commands, const kiv_os::THandle &stdinHandle, const kiv_os::THandle &stdoutHandle);
	bool validCommands(std::vector<Command> commands);

private:
	bool validCommand(const Command &command);
	bool validFileName(const std::string &filename);
};