#pragma once
#include "command.h"
#include <string>
#include <vector>

constexpr std::string_view INPUT_FOR_COMMAND = "<";
constexpr std::string_view OUTPUT_FOR_COMMAND = ">";

class Command
{
public:
    Command();
    static std::vector<Command> Parse_Input(const std::string& input);
    static std::vector<Command> Parse_Commands(std::vector<std::string>& commands);
    static Command Parse_Command(std::string command);
    std::string Get_Parameters();
    std::string command_name;
    std::vector<std::string> parameters;
    std::string input_filename;
    std::string output_filename;
    bool redirect_pipe;
    bool has_input_file;
    bool has_output_file;
};

