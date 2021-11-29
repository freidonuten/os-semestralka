﻿#include "command_executor.h"
#include "rtl.h"
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
	std::deque<kiv_os::THandle> used_pipes;

	auto index = size_t(0);

	const auto consume_first_pipe = [&pipe_queue, &used_pipes]() {
		const auto handle = pipe_queue.front();
		pipe_queue.pop_front();
		used_pipes.push_back(handle);
		return handle;
	};


	for (Command command : commands) {
		kiv_os::THandle process_handle;
		kiv_os::THandle handle_in = stdin_handle;
		kiv_os::THandle handle_out = stdout_handle;

		if (strcmp(command.command_name.c_str(), "cd") == 0) {
			kiv_os::NOS_Error ret_code = kiv_os_rtl::Set_Working_Dir(command.Get_Parameters());
			if (ret_code != kiv_os::NOS_Error::Success) {
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
			auto error = kiv_os_rtl::Create_Pipe(pipe_handles);
			if (error != kiv_os::NOS_Error::Success) {
				auto message = utils::get_error_message(error);
				kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
				kiv_os_rtl::Exit(2);
				return;
			}
			pipe_queue.push_back(pipe_handles[1]);
			pipe_queue.push_back(pipe_handles[0]);
		}


		if (index > 0) {
			handle_in = consume_first_pipe();
		}

		if (index < commands.size() - 1) {
			handle_out = consume_first_pipe();
		}

		const auto is_running = kiv_os_rtl::Create_Process(
			command.command_name, command.Get_Parameters(), handle_in, handle_out, process_handle
		);

		if (is_running != kiv_os::NOS_Error::Success) {
			auto message = utils::get_error_message(is_running);
			kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
			kiv_os_rtl::Exit(2);
			return;
		}

		handles.push_back(process_handle);
		index++;
	}

	// Wait for each process and close its opened pipes
	auto const close_first_pipe = [&used_pipes]() {
		const auto pipe_handle = used_pipes.front();
		used_pipes.pop_front();
		kiv_os_rtl::Close_Handle(pipe_handle);
	};

	index = 0;
	for (const auto handle : handles) {
		uint16_t exit_code;
		kiv_os_rtl::Read_Exit_Code(handle, exit_code);

		if (index > 0) {
			close_first_pipe;
		}

		if (index < commands.size() - 1) {
			close_first_pipe;
		}

		++index;
	}
}