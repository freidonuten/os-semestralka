#include "command.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>


// Parse input command to single commands
std::vector<Command> Command::parseInput(const std::string& input) {
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
        if (command.empty()) {
            continue;
        }
        Command newCommand = parseCommand(command);
        if (newCommand.command_name.empty())
            continue;
        if (!firstCommand) {
            firstCommand = true;
        } else {
            newCommand.redirect_pipe = true;
        }
        parsedCommands.push_back(newCommand);
    }
    return parsedCommands;
}

Command Command::parseCommand(const std::string& command) {
    const char* p_command = command.data();
    std::stringstream stream(p_command);
    std::string token;
    std::string filename;
    Command newCommand;
    if (!(stream >> token)) {
        return {};
    }
    newCommand.command_name = token;
    token.clear();
    while (stream >> token) {
        if (strcmp(token.c_str(), INPUT_FOR_COMMAND) == 0) {
            stream >> token;
            newCommand.input = true;
            newCommand.input_filename = token;
        }
        else if (strcmp(token.c_str(), OUTPUT_FOR_COMMAND) == 0) {
            stream >> token;
            newCommand.output = true;
            newCommand.output_filename = token;
        }
        else {
            newCommand.parameters.push_back(token);
        }
        token.clear();
    }
    return newCommand;
}
