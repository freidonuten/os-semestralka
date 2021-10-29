#include "command_executor.h"
#include "rtl.h"
#include "global.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <map>

void cd(const std::string & path, kiv_os::THandle out) {
	bool isSetDir = kiv_os_rtl::Set_Working_Dir(path);
	if (!isSetDir) {
		size_t chars = 0;
		kiv_os_rtl::Write_File(out, ERROR_MSG_DIR_NOT_FOUND.data(), ERROR_MSG_DIR_NOT_FOUND.size(), chars);
	}
}

void CommandExecutor::executeCommand(std::vector<Command> commands, const kiv_os::THandle& stdinHandle, const kiv_os::THandle& stdoutHandle) {
	std::map<size_t, kiv_os::THandle> inPipes;
	std::map<size_t, kiv_os::THandle> outPipes;
	std::vector<kiv_os::THandle> handles;
	size_t commandCounter = 0;
	

	if (!validCommands(commands)) {
		size_t counter = 0;
		kiv_os_rtl::Write_File(stdinHandle, ERROR_MSG_INVALID_COMMAND.data(), ERROR_MSG_INVALID_COMMAND.size(), counter);
		return;
	}

	for (Command command : commands) {
		kiv_os::THandle processHandle;
		kiv_os::THandle pipeHandles[2];
		kiv_os::THandle handleIn = stdinHandle;
		kiv_os::THandle handleOut = stdoutHandle;
		if (command.hasInputFile) {
			kiv_os_rtl::Open_File(command.inputFileName, kiv_os::NFile_Attributes::System_File, kiv_os::NOpen_File::fmOpen_Always, handleIn);
		}

		if (command.hasOutputFile) {
			kiv_os_rtl::Open_File(command.outputFileName, kiv_os::NFile_Attributes::System_File, static_cast<kiv_os::NOpen_File>(0), handleOut);
		}

		if (command.redirectPipe) {
			kiv_os_rtl::Create_Pipe(pipeHandles);
			inPipes.insert(std::pair<size_t, kiv_os::THandle>(commandCounter, pipeHandles[0]));
			outPipes.insert(std::pair<size_t, kiv_os::THandle>(commandCounter, pipeHandles[1]));
			handleOut = pipeHandles[1];
		}

		//?????
		if ((commandCounter > 0) && (inPipes.find(commandCounter - 1) != inPipes.end())) {
			handleIn = pipeHandles[commandCounter - 1];
		}

		kiv_os_rtl::Create_Process(command.commandName, command.getParameters(), handleIn, handleOut, processHandle);
		handles.push_back(processHandle);
		commandCounter++;
	}
}

bool CommandExecutor::validCommands(std::vector<Command> commands) {
	bool returnCode = true;
	std::for_each(commands.begin(), commands.end(), [=, &returnCode](Command command) -> void {
		if (!(std::find(allCommands.begin(), allCommands.end(), command.commandName) != allCommands.end())) {
			//TODO vymazat
			//std::cout << "Command dont exist: " << command.command_name;
			returnCode = false;
			return;
		} else if (command.hasInputFile && command.inputFileName.empty()) {
			//TODO vymazat
			//std::cout << "Command miss input: " << command.command_name;
			returnCode = false;
			return;
		} else if (command.hasOutputFile && command.outputFileName.empty()) {
			//TODO vymazat
			//std::cout << "Command miss output: " << command.command_name;
			returnCode = false;
			return;
		} else if (!validCommand(command)) {
			//TODO vymazat
			//std::cout << "input/output filename: " << command.command_name;
			returnCode = false;
			return;
		}});
	return returnCode;
}

bool CommandExecutor::validCommand(const Command& command) {
	const bool input = validFileName(command.inputFileName);
	const bool output = validFileName(command.outputFileName);
	return (input & output);
}

bool CommandExecutor::validFileName(const std::string& filename) {
	if (!filename.empty()) {
		if (filename == INPUT_FOR_COMMAND || filename == OUTPUT_FOR_COMMAND) {
			return false;
		}
		else {
			bool matched = std::regex_match(filename, fileNameRegex);
			return !matched;
		}
	}
	return true;
}