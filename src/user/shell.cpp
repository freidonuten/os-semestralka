#include "shell.h"
#include "rtl.h"
#include "command.h"
#include "command_executor.h"
#include "global.h"
#include <string>
#include <vector>

size_t printNewLinePrompt(const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	std::string currentDir = "";
	size_t currentDirSize = currentDir.size();
	size_t counter = 0;
	kiv_os_rtl::Get_Working_Dir(currentDir, currentDirSize, counter);

	// Zapis do konzole C:\>
	kiv_os_rtl::Write_File(stdout_handle, prompt.data(), prompt.size(), counter);
	kiv_os_rtl::Write_File(stdout_handle, beak.data(), beak.size(), counter);
	kiv_os_rtl::Write_File(stdout_handle, currentDir.data(), currentDir.size(), counter);
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

	// Console loop
	while(1) {
		printNewLinePrompt(std_in, std_out);

		if (kiv_os_rtl::Read_File(std_in, buffer, buffer_size, counter)) {
			if ((counter > 0) && (counter == buffer_size)) counter--;
			buffer[counter] = 0;	//udelame z precteneho vstup null-terminated retezec

			std::string input_command(buffer);
			commands = Command::Parse_Input(input_command);

			// Kontrola zda byl zadan nejaky prikaz
			if (!commands.size()) {
				kiv_os_rtl::Write_File(std_out, new_line.data(), new_line.size(), counter);
				continue;
			}
			command_executor.Execute_Command(commands, std_in, std_out);
			
			if (strcmp(buffer, "exit") == 0) {
				break;
			}
			/*
			// vypise soucasny command
			kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
			kiv_os_rtl::Write_File(std_out, buffer, strlen(buffer), counter);	//a vypiseme ho
			kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
			*/
		}
		kiv_os_rtl::Write_File(std_out, new_line.data(), new_line.size(), counter);
		commands.clear();
	}
	return 0;	
}