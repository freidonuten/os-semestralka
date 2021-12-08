#include "command_executor.h"
#include "rtl_wrappers.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <array>
#include <regex>
#include <map>
#include <deque>

void close_handles(std::vector<kiv_os::THandle>& opened_files) {
	for (auto handle : opened_files) {
		rtl::Close_Handle(handle);
	}
}

void cwd(const kiv_os::THandle out, const std::string& path) {
		if (!path.size()) {
			auto cwd_buffer = std::array<char, 256>{};
			const auto [size, error] = rtl::Get_Working_Dir(cwd_buffer);

			rtl::Write_File(out, std::string_view(cwd_buffer.data(), size));
			rtl::Write_File(out, new_line);

			return;
		} 

		kiv_os::NOS_Error ret_code = rtl::Set_Working_Directory(path);
		if (ret_code != kiv_os::NOS_Error::Success) {
			rtl::Write_File(out, ERROR_MSG_DIR_NOT_FOUND);
		}
}

void CommandExecutor::Execute_Command(std::vector<Command> commands, const kiv_os::THandle& stdin_handle, const kiv_os::THandle& stdout_handle) {
	std::vector<kiv_os::THandle> handles;
	size_t chars_written = 0;
	kiv_os::NOS_Error error;

	std::deque<kiv_os::THandle> pipe_queue;
	std::deque<kiv_os::THandle> used_pipes;

	std::vector<kiv_os::THandle> opened_files;

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

		if (command.command_name == "cd") {
			cwd(handle_out, command.Get_Parameters());
			return;
		}

		if (command.has_input_file) {
			std::tie(handle_in, error) = rtl::Open_File(command.input_filename, utils::get_file_attrs());
			if (error != kiv_os::NOS_Error::Success) {
				rtl::Write_File(stdout_handle, utils::get_error_message(error));
				close_handles(opened_files);
				return;
			}
			else {
				opened_files.push_back(handle_in);
			}
		}

		if (command.has_output_file) {
			std::tie(handle_out, error) = rtl::Open_File(command.output_filename, utils::get_file_attrs(), static_cast<kiv_os::NOpen_File>(0));
			if (error != kiv_os::NOS_Error::Success) {
				rtl::Write_File(stdout_handle, utils::get_error_message(error));
				close_handles(opened_files);
				return;
			}
			else {
				opened_files.push_back(handle_out);
			}
		}

		else if (index + 2 <= commands.size()) {
			kiv_os::THandle pipe_handles[2];
			error = kiv_os_rtl::Create_Pipe(pipe_handles);
			if (error != kiv_os::NOS_Error::Success) {
				rtl::Write_File(stdout_handle, utils::get_error_message(error));
				close_handles(opened_files);
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

		std::tie(process_handle, error) = rtl::Create_Process(command.command_name, command.Get_Parameters(), handle_in, handle_out);

		if (error != kiv_os::NOS_Error::Success) {
			rtl::Write_File(stdout_handle,  utils::get_error_message(error));
			close_handles(opened_files);
			return;
		}

		handles.push_back(process_handle);
		index++;
	}

	// Wait for each process and close its opened pipes
	auto const close_first_pipe = [&used_pipes]() {
		const auto pipe_handle = used_pipes.front();
		used_pipes.pop_front();
		rtl::Close_Handle(pipe_handle);
	};

	index = 0;
	for (const auto handle : handles) {
		rtl::Read_Exit_Code(handle);

		if (index > 0) {
			close_first_pipe();
		}

		if (index < commands.size() - 1) {
			close_first_pipe();
		}

		++index;
	}

	close_handles(opened_files);
}