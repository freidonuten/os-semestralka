#include "command_executor.h"
#include "rtl.h"
#include "global.h"
#include "utils.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <map>
#include <deque>

void exit(Command command, kiv_os::NOS_Error error, kiv_os::THandle stdout_handle, size_t chars_written) {

}

void CommandExecutor::Execute_Command(std::vector<Command> commands, const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	std::vector<kiv_os::THandle> handles;
	size_t chars_written = 0;
	kiv_os::NOS_Error error;

	std::deque<kiv_os::THandle> pipe_queue;
	auto index = size_t(0);

	for (Command command : commands) {
		kiv_os::THandle process_handle;
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
			error = kiv_os_rtl::Open_File(command.input_filename, utils::get_file_attrs(), kiv_os::NOpen_File::fmOpen_Always, handle_in);
			if (error != kiv_os::NOS_Error::Success) {
				//TODO zavolat metodu exit a vse poctive zavrit
				auto message = utils::get_error_message(error);
				kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
				kiv_os_rtl::Exit(2);
				return;
			}
		}

		if (command.has_output_file) {
			error = kiv_os_rtl::Open_File(command.output_filename, utils::get_file_attrs(), static_cast<kiv_os::NOpen_File>(0), handle_out);
			if (error != kiv_os::NOS_Error::Success) {
				//TODO zavolat metodu exit a vse poctive zavrit
				auto message = utils::get_error_message(error);
				kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
				kiv_os_rtl::Exit(2);
				return;
			}
		}

		else if (index <= commands.size() - 2) {
			kiv_os::THandle pipe_handles[2];
			kiv_os_rtl::Create_Pipe(pipe_handles);
			pipe_queue.push_back(pipe_handles[1]);
			pipe_queue.push_back(pipe_handles[0]);
		}

		if (index < commands.size() - 1) {
			handle_out = pipe_queue.front();
			pipe_queue.pop_front();
		}

		if (index > 0) {
			handle_in = pipe_queue.front();
			pipe_queue.pop_front();
		}

		const auto is_running = kiv_os_rtl::Create_Process(
			command.command_name, command.Get_Parameters(), handle_in, handle_out, process_handle
		);

		// FIXME tady čekat nebudeme
		if (is_running) {
		} else if (kiv_os_rtl::Last_Error == kiv_os::NOS_Error::Out_Of_Memory) {
			kiv_os_rtl::Write_File(handle_out, ERROR_OUT_OF_MEMORY.data(), ERROR_OUT_OF_MEMORY.size(), chars_written);
			kiv_os_rtl::Exit(1);
			return;
		}

		if (command.has_input_file) {
			kiv_os_rtl::Close_Handle(handle_in);
		}

		if (command.has_output_file) {
			kiv_os_rtl::Close_Handle(handle_out);
		}

		//TODO zavrit pipy (tomu nerozumim -stepan)


		handles.push_back(process_handle);
		index++;
	}

	uint16_t exit_code;
	kiv_os_rtl::Read_Exit_Code(handles.back(), exit_code);
}