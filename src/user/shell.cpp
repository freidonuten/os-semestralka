#include "shell.h"
#include "rtl.h"
#include "rtl_wrappers.h"
#include "command.h"
#include "command_executor.h"
#include "global.h"
#include <string>
#include <vector>
#include <array>

bool is_echo_on = true;

void Print_Newline_Prompt(const kiv_os::THandle& stdout_handle) {
	auto cwd_buffer = std::array<char, 256>{};
	const auto [size, error] = rtl::Get_Working_Dir(cwd_buffer);

	rtl::Write_File(stdout_handle, prompt);
	rtl::Write_File(stdout_handle, std::string_view(cwd_buffer.data(), size));
	rtl::Write_File(stdout_handle, beak);
}

size_t __stdcall shell(const kiv_hal::TRegisters &regs) {
	const auto std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const auto std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	auto buffer = std::array<char, 256>{};
	auto command_executor = CommandExecutor();

	rtl::Write_File(std_out, welcome_text);

	while(1) {
		if (is_echo_on) {
			Print_Newline_Prompt(std_out);
		}

		const auto [count, eof, error] = rtl::Read_File(std_in, buffer);
		if (error == kiv_os::NOS_Error::Success) {
			const auto input_command = std::string(buffer.data(), count);
			auto commands = Command::Parse_Input(input_command);

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