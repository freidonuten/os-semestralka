#include "command_executor.h"
#include "rtl.h"
#include "global.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <map>

void CommandExecutor::Execute_Command(std::vector<Command> commands, const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	std::map<size_t, kiv_os::THandle> in_pipes;
	std::map<size_t, kiv_os::THandle> out_pipes;
	std::vector<kiv_os::THandle> handles;
	size_t command_counter = 0;
	size_t chars_written = 0;

	for (Command command : commands) {
		kiv_os::THandle process_handle;
		kiv_os::THandle pipe_handles[2];
		kiv_os::THandle handle_in = stdin_handle;
		kiv_os::THandle handle_out = stdout_handle;

		if (strcmp(command.command_name.c_str(), "cd") == 0) {
			bool is_set_dir = kiv_os_rtl::Set_Working_Dir(command.Get_Parameters());
			if (!is_set_dir) {
				size_t chars = 0;
				kiv_os_rtl::Write_File(handle_out, ERROR_MSG_DIR_NOT_FOUND.data(), ERROR_MSG_DIR_NOT_FOUND.size(), chars);
			}
			return;
		}

		if (command.has_input_file) {
			kiv_os_rtl::Open_File(command.input_filename, static_cast<uint8_t>(kiv_os::NFile_Attributes::System_File), kiv_os::NOpen_File::fmOpen_Always, handle_in);
			if (handle_in == invalid_file_handle) {
				kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
				kiv_os_rtl::Exit(2);
				return;
			}
		}

		if (command.has_output_file) {
			kiv_os_rtl::Open_File(command.output_filename, static_cast<uint8_t>(kiv_os::NFile_Attributes::System_File), static_cast<kiv_os::NOpen_File>(0), handle_out);
			if (handle_in == invalid_file_handle) {
				kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
				kiv_os_rtl::Exit(2);
				return;
			}
		}

		if (command.redirect_pipe) {
			kiv_os_rtl::Create_Pipe(pipe_handles);
			in_pipes.insert(std::pair<size_t, kiv_os::THandle>(command_counter, pipe_handles[0]));
			out_pipes.insert(std::pair<size_t, kiv_os::THandle>(command_counter, pipe_handles[1]));
			handle_out = pipe_handles[1];
		}

		if ((command_counter > 0) && (in_pipes.find(command_counter - 1) != in_pipes.end())) {
			handle_in = in_pipes[command_counter - 1];
		}

		const auto is_running = kiv_os_rtl::Create_Process(
			command.command_name, command.Get_Parameters(), handle_in, handle_out, process_handle
		);

		if (is_running) {
			uint16_t exit_code;
			kiv_os_rtl::Read_Exit_Code(process_handle, exit_code);
		} else if (kiv_os_rtl::Last_Error == kiv_os::NOS_Error::Out_Of_Memory) {
			kiv_os_rtl::Write_File(handle_out, ERROR_OUT_OF_MEMORY.data(), ERROR_OUT_OF_MEMORY.size(), chars_written);
			kiv_os_rtl::Exit(1);
			return;
		}

		handles.push_back(process_handle);
		command_counter++;
	}
}