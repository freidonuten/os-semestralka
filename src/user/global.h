#pragma once

#include <regex>
#include <string>
#include <string_view>

/*
	Error messages
*/
constexpr std::string_view ERROR_MSG_DIR_NOT_FOUND = "Cannot found directory.\n";
constexpr std::string_view ERROR_MSG_INVALID_COMMAND = "Invalid command.\n";
constexpr std::string_view welcomeText= "Vitejte v kostre semestralni prace z KIV/OS.\n" \
										"Shell zobrazuje echo zadaneho retezce. Prikaz exit ukonci shell.\n";
constexpr std::string_view prompt = "C:\\";
constexpr std::string_view beak = ">";
constexpr std::string_view newLine = "\n";

/*
	Variables
*/
const std::vector<std::string> allCommands = {
		"echo", "cd", "dir", "md", "rd", "type", "exit",
		"find", "sort", "tasklist", "shutdown", "rgen", "freq"
};
const std::regex fileNameRegex("[a-zA-Z][a-zA-Z0-9]*\\.[a-zA-Z0-9]+");
constexpr int BUFFER_SIZE = 1024;