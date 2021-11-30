#include "shell.h"
#include "rtl.h"
#include "command.h"
#include "command_executor.h"
#include "global.h"
#include <string>
#include <vector>
#include <array>

bool is_echo_on = true;

size_t Print_Newline_Prompt(const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	char currentDirBuffer[256] = "";
	size_t counter = 0;
	kiv_os_rtl::Get_Working_Dir(currentDirBuffer, 256, counter);
	const auto currentDir = std::string(currentDirBuffer);

	// Zapis do konzole C:\>
	if (is_echo_on) {
		kiv_os_rtl::Write_File(stdout_handle, prompt);
		kiv_os_rtl::Write_File(stdout_handle, currentDir);
		kiv_os_rtl::Write_File(stdout_handle, beak);
	}

	return counter;
}

size_t __stdcall shell(const kiv_hal::TRegisters &regs) {
	const auto std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const auto std_out = static_cast<kiv_os::THandle>(regs.rbx.x);
	const auto buffer_size = size_t(256);

	auto buffer = std::array<char, 256>{};
	auto command_executor = CommandExecutor();

	kiv_os_rtl::Write_File(std_out, welcome_text);

	while(1) {
		Print_Newline_Prompt(std_in, std_out);

		const auto [count, error] = kiv_os_rtl::Read_File(std_in, buffer);
		if (error == kiv_os::NOS_Error::Success) {
			const auto input_command = std::string(buffer.data(), count);
			auto commands = Command::Parse_Input(input_command);

			kiv_os_rtl::Write_File(std_out, new_line);
			if (!commands.size()) {
				continue;
			}

			if (commands.front().command_name == "@echo") {
				if (commands.front().Get_Parameters() == "off") {
					is_echo_on = false;
				} else if (commands.front().Get_Parameters() == "on") {
					is_echo_on = true;
				}
				continue;
			} else if (commands.front().command_name == "exit") {
				break;
			}
			
			command_executor.Execute_Command(commands, std_in, std_out);
		}
	}
	return 0;	
}