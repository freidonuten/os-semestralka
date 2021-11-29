#include "shell.h"
#include "rtl.h"
#include "command.h"
#include "command_executor.h"
#include "global.h"
#include <string>
#include <vector>

bool is_echo_on = true;

size_t Print_Newline_Prompt(const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	char currentDirBuffer[256] = "";
	size_t counter = 0;
	kiv_os_rtl::Get_Working_Dir(currentDirBuffer, 256, counter);
	std::string currentDir = std::string(currentDirBuffer);

	// Zapis do konzole C:\>
	if (is_echo_on) {
		kiv_os_rtl::Write_File(stdout_handle, prompt.data(), prompt.size(), counter);
		kiv_os_rtl::Write_File(stdout_handle, currentDir.data(), currentDir.size(), counter);
		kiv_os_rtl::Write_File(stdout_handle, beak.data(), beak.size(), counter);
	}
	return counter;
}

size_t __stdcall shell(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);
	const size_t buffer_size = 256;
	char buffer[buffer_size];
	size_t counter;

	kiv_os_rtl::Write_File(std_out, welcome_text.data(), welcome_text.size(), counter);
	std::vector<Command> commands;
	CommandExecutor command_executor;

	while(1) {
		Print_Newline_Prompt(std_in, std_out);

		if (kiv_os_rtl::Read_File(std_in, buffer, buffer_size, counter) == kiv_os::NOS_Error::Success) {
			if ((counter > 0) && (counter == buffer_size)) {
				counter--;
			}
			buffer[counter] = 0;
			std::string input_command(buffer);
			commands = Command::Parse_Input(input_command);

			kiv_os_rtl::Write_File(std_out, new_line.data(), new_line.size(), counter);
			if (!commands.size()) {
				continue;
			}

			if (commands.front().command_name == "@echo") {
				if (commands.front().Get_Parameters() == "off") {
					is_echo_on = false;
				} else if (commands.front().Get_Parameters() == "on") {
					is_echo_on = true;
				}
			} else if (commands.front().command_name == "exit") {
				break;
			}
			
			command_executor.Execute_Command(commands, std_in, std_out);
		}
		commands.clear();
	}
	return 0;	
}