#include "command.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

Command::Command() {
    this->commandName = "";
    this->inputFileName = "";
    this->outputFileName = "";
    this->redirectPipe = false;
    this->hasInputFile = false;
    this->hasOutputFile = false;
}

// Parse input command to single commands
std::vector<Command> Command::parseInput(std::string const& input) {
    std::vector<std::string> commandsString;
    std::vector<Command> commands;
    std::istringstream input_stream(input);
    std::string token;
    bool firstCommand = false;
    while (std::getline(input_stream, token, '|')) {
        commandsString.push_back(token);
    }
    commands = parseCommands(commandsString);
    
    return commands;
}

// Parse single command to base command and parameters
std::vector<Command> Command::parseCommands(const std::vector<std::string>& commands) {
    std::vector<Command> parsedCommands;
    bool firstCommand = false;
    for (auto& command : commands) {
        if (command.empty())
            continue;
        Command newCommand = parseCommand(command);
        if (newCommand.commandName.empty())
            continue;
        if (!firstCommand) {
            firstCommand = true;
        } else {
            newCommand.redirectPipe = true;
        }
        parsedCommands.push_back(newCommand);
    }
    return parsedCommands;
}

Command Command::parseCommand(std::string command) {
    const char* p_command = command.data();
    std::stringstream stream(p_command);
    std::string token;
    std::string filename;
    Command newCommand;
    if (!(stream >> token)) {
        return {};
    }
    newCommand.commandName = token;
    if (token.find("echo") != std::string::npos) {
        if (token.rfind("@", 0) == 0) {
            newCommand.commandName = "echo";
            newCommand.specialParameters.push_back("@");
        }
    }
    token.clear();
    while (stream >> token) {
        if (strcmp(token.c_str(), INPUT_FOR_COMMAND.data()) == 0) {
            stream >> token;
            newCommand.hasInputFile = true;
            newCommand.inputFileName = token;
        }
        else if (strcmp(token.c_str(), OUTPUT_FOR_COMMAND.data()) == 0) {
            stream >> token;
            newCommand.hasOutputFile = true;
            newCommand.outputFileName = token;
        }
        else {
            newCommand.parameters.push_back(token);
        }
        token.clear();
    }
    return newCommand;
}

std::string Command::getParameters() {
    std::string allParameters;
    for (auto it = parameters.begin(); it != parameters.end(); it++) {
        allParameters.append(*it);
        if (it != parameters.end()) {
            allParameters.append(" ");
        }
    }
    return allParameters;
}
