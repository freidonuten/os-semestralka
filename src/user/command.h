#pragma once
#include "command.h"
#include <string>
#include <vector>

#define INPUT_FOR_COMMAND "<"
#define OUTPUT_FOR_COMMAND ">"

class Command
{
public:
    Command();
    static std::vector<Command> parseInput(const std::string& input);
    static std::vector<Command> parseCommands(const std::vector<std::string>& commands);
    static Command parseCommand(const std::string& command);
    std::string command_name = "";
    std::vector<std::string> parameters;
    std::string input_filename = "";
    std::string output_filename = "";
    bool redirect_pipe = false;
    bool input = false;
    bool output = false;
};

