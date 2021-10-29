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
    static std::vector<Command> parseInput(const std::string& input);
    static std::vector<Command> parseCommands(const std::vector<std::string>& commands);
    static Command parseCommand(std::string command);
    std::string getParameters();
    std::string commandName;
    std::vector<std::string> parameters;
    std::vector<std::string> specialParameters;
    std::string inputFileName;
    std::string outputFileName;
    bool redirectPipe;
    bool hasInputFile;
    bool hasOutputFile;
};

