#pragma once

#include <regex>
#include <string>
#include <string_view>

/*
	Error messages
*/
constexpr std::string_view ERROR_MSG_DIR_NOT_FOUND = "Cannot found directory.\n";
constexpr std::string_view ERROR_MSG_INVALID_COMMAND = "Invalid command.\n";
constexpr std::string_view ERROR_MSG_INVALID_COMMAND_ARGUMENT = "Invalid command argument.\n";
constexpr std::string_view ERROR_MSG_CANT_OPEN_FILE = "File can not be opened.\n";



/*
	Variables
*/
constexpr int BUFFER_SIZE = 1024;
constexpr std::string_view welcome_text = "Vitejte v kostre semestralni prace z KIV/OS.\n" \
										  "Shell zobrazuje echo zadaneho retezce. Prikaz exit ukonci shell.\n";
constexpr std::string_view prompt = "C:\\";
constexpr std::string_view beak = ">";
constexpr std::string_view new_line = "\n";
constexpr kiv_os::THandle invalid_file_handle = static_cast<kiv_os::THandle>(-1);
constexpr int eof = static_cast<int>(kiv_hal::NControl_Codes::SUB);
constexpr int eot = static_cast<int>(kiv_hal::NControl_Codes::EOT);
constexpr int etx = static_cast<int>(kiv_hal::NControl_Codes::ETX);