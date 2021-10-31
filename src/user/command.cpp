#include "command.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

Command::Command() {
    this->command_name = "";
    this->input_filename = "";
    this->output_filename = "";
    this->redirect_pipe = false;
    this->has_input_file = false;
    this->has_output_file = false;
}

// Parse input command to single commands
std::vector<Command> Command::Parse_Input(std::string const& input) {
    std::vector<std::string> commands_string;
    std::vector<Command> commands;
    std::istringstream input_stream(input);
    std::string token;
    while (std::getline(input_stream, token, '|')) {
        commands_string.push_back(token);
    }
    commands = Parse_Commands(commands_string);
    return commands;
}

// Parse single command to base command and parameters
std::vector<Command> Command::Parse_Commands(const std::vector<std::string>& commands) {
    std::vector<Command> parsed_commands;
    bool first_command = false;
    for (auto& command : commands) {
        if (command.empty())
            continue;
        Command new_command = Parse_Command(command);
        if (new_command.command_name.empty())
            continue;
        if (!first_command) {
            first_command = true;
        } else {
            new_command.redirect_pipe = true;
        }
        parsed_commands.push_back(new_command);
    }
    return parsed_commands;
}

Command Command::Parse_Command(std::string command) {
    const char* p_command = command.data();
    std::stringstream stream(p_command);
    std::string token;
    std::string filename;
    Command new_command;
    if (!(stream >> token)) {
        return {};
    }
    new_command.command_name = token;
    if (token.find("echo") != std::string::npos) {
        if (token.rfind("@", 0) == 0) {
            new_command.command_name = "echo";
            new_command.parameters.push_back("@");
        }
    }
    token.clear();
    while (stream >> token) {
        if (strcmp(token.c_str(), INPUT_FOR_COMMAND.data()) == 0) {
            stream >> token;
            new_command.has_input_file = true;
            new_command.input_filename = token;
        }
        else if (strcmp(token.c_str(), OUTPUT_FOR_COMMAND.data()) == 0) {
            stream >> token;
            new_command.has_output_file = true;
            new_command.output_filename = token;
        }
        else {
            new_command.parameters.push_back(token);
        }
        token.clear();
    }
    return new_command;
}

std::string Command::Get_Parameters() {
    std::string all_parameters;
    for (auto it = parameters.begin(); it != parameters.end(); it++) {
        all_parameters.append(*it);
        if (it != parameters.end()) {
            all_parameters.append(" ");
        }
    }
    return all_parameters;
}
