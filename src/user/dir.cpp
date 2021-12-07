#include "dir.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>

std::pair<size_t, size_t> load_entries(const kiv_os::THandle handle, std::vector<kiv_os::TDir_Entry>& target) {
	auto buffer = std::array<char, dir_size>();
	auto files_counter = size_t(0);
	auto dir_counter = size_t(0);
	auto position = size_t(0);

	kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, position);
	target.reserve(16);
	while (true) {
		const auto [count, err] = rtl::Read_File(handle, buffer);

		if (!count) {
			break;
		}

		kiv_os::TDir_Entry entry = *(reinterpret_cast<kiv_os::TDir_Entry*>(buffer.data()));
		utils::is_dir(static_cast<uint8_t>(entry.file_attributes))
			? ++dir_counter
			: ++files_counter;

		target.push_back(entry);
	}

	constexpr auto directory_first_predicate = [](const auto& a, const auto& b) {
		return utils::is_dir(static_cast<uint8_t>(a.file_attributes))
			 > utils::is_dir(static_cast<uint8_t>(b.file_attributes));
	};

	std::sort(target.begin(), target.end(), directory_first_predicate);

	return { files_counter, dir_counter };
}

std::pair<bool, std::string> parse_args(const std::string args) {
	auto iss = std::istringstream(args);
	auto results = std::vector<std::string>(
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>()
		);

	if (results.size() == 1) {
		if (results[0] == "/S") {
			return { true, "." };
		}
		return { false, results[0] };
	}

	if (results.size() == 2 && results[0] == "/S") {
		return { true, results[1] };
	}

	return { false, "." };
}

std::string get_output_string_path(std::string current_path, std::string file, bool isDir) {
	char file_type = isDir == true ? '+' : ' ';
	std::string delimiter = "\\";
	std::string path(file_type + current_path + delimiter + file);
	return path;
}

std::string get_path(std::string current_path, std::string file) {
	std::string path(current_path + "\\" + file);
	return path;
}

size_t recursive_search(std::vector<kiv_os::TDir_Entry>& entries, std::vector<std::string>& all_entries, std::string path, kiv_os::THandle stdout_handle, size_t& f_count, size_t& d_count) {
	for (const auto entry : entries) {
		if (utils::is_file(static_cast<uint8_t>(entry.file_attributes))) {
			all_entries.push_back(get_output_string_path(path, entry.file_name, false));
			continue;
		}
		const auto [current_file_handle, error] = rtl::Open_File(get_path(path, entry.file_name), utils::get_dir_attrs());
		if (error != kiv_os::NOS_Error::Success) {
			rtl::Write_File(stdout_handle, utils::get_error_message(error));
			KIV_OS_EXIT(2);
		}
		all_entries.push_back(get_output_string_path(path, entry.file_name, true));
		auto sub_entries = std::vector<kiv_os::TDir_Entry>();
		const auto [file_count, dir_count] = load_entries(current_file_handle, sub_entries);
		f_count += file_count;
		d_count += dir_count;
		recursive_search(sub_entries, all_entries, get_path(path, entry.file_name), stdout_handle, f_count, d_count);
		rtl::Close_Handle(current_file_handle);
	}
	return 0;
}

size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	size_t f_count = 0;
	size_t d_count = 0;

	// parse arguments
	auto args = std::string(reinterpret_cast<const char*>(regs.rdi.r));
	const auto [recurse, filename] = parse_args(args);

	// open file
	auto [file_handle, error] = rtl::Open_File(filename, utils::get_dir_attrs());

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	// load entries
	auto entries = std::vector<kiv_os::TDir_Entry>();
	auto all_entries = std::vector<std::string>();
	auto [file_count, dir_count] = load_entries(file_handle, entries);
	f_count += file_count;
	d_count += dir_count;

	if (recurse) {
		std::string path;
		size_t chars_written = 0;
		kiv_os_rtl::Get_Working_Dir(path.data(), BUFFER_SIZE, chars_written);
		recursive_search(entries, all_entries, path, stdout_handle, f_count, d_count);
	}
	else {
		for (const auto entry : entries) {
			std::string file_type = utils::is_dir(static_cast<uint8_t>(entry.file_attributes)) ? "+" : " ";
			std::string entry_file(file_type + entry.file_name);
			all_entries.push_back(entry_file);
		}
	}

	// print
	std::ostringstream dir_content;
	dir_content
		<< "Directories: " << d_count << new_line
		<< "Files: " << f_count << new_line;

	for (const auto entry : all_entries) {
		dir_content
			<< entry
			<< new_line;
	}

	rtl::Write_File(stdout_handle, dir_content.str());
	rtl::Close_Handle(file_handle);

	KIV_OS_EXIT(0);
}
