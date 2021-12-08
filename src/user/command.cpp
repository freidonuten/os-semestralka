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
std::vector<Command> Command::Parse_Input(const std::string& input) {
    std::vector<std::string> commands_string;
    std::istringstream input_stream(input);
    std::string token;

    while (std::getline(input_stream, token, '|')) {
        commands_string.push_back(token);
    }

    return Parse_Commands(commands_string);
}

// Parse single command to base command and parameters
std::vector<Command> Command::Parse_Commands(std::vector<std::string>& commands) {
    std::vector<Command> parsed_commands;
    bool first_command = false;

    if (commands.empty()) {
        return {};
    }

    for (const auto& command : commands) {
        if (command.empty()) {
            continue;
        }
        Command new_command = Parse_Command(command);
        if (new_command.command_name.empty()) {
            continue;
        }
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
    std::stringstream stream(command);
    std::string token;
    std::string filename;
    Command new_command;
    if (!(stream >> token)) {
        return {};
    }

    const auto set_file = [&token, &stream](auto& bool_flag, auto& target, const auto matcher) {
        if (bool_flag = token[0] == matcher) {
            if (token.size() == 1) {
                stream >> token;
                target = token;
            } else {
                target = token.substr(1, token.size() - 1);
            }
        }
        return bool_flag;
    };

    new_command.command_name = token;
    token.clear();
    while (stream >> token) {
        const auto file_found = set_file(new_command.has_input_file, new_command.input_filename, INPUT_FOR_COMMAND)
            || set_file(new_command.has_output_file, new_command.output_filename, OUTPUT_FOR_COMMAND);

        if (!file_found) {
            new_command.parameters.push_back(token);
		}

        token.clear();
    }

    return new_command;
}

std::string Command::Get_Parameters() {
    std::string all_parameters;
    for (auto it = parameters.begin(); it != parameters.end(); it++) {
        if (it != parameters.begin()) {
            all_parameters.append(" ");
        }
        all_parameters.append(*it);
    }
    return all_parameters;
}
