#include "dir.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>

// Return files and directories from directory passed in argument
std::pair<size_t, size_t> load_entries(const kiv_os::THandle handle, std::vector<kiv_os::TDir_Entry>& target) {
	auto buffer = std::array<char, dir_size>();
	auto files_counter = size_t(0);
	auto dir_counter = size_t(0);
	auto position = size_t(0);

	// Set seek in directory file
	kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, position);
	target.reserve(16);
	while (true) {
		const auto [count, eof, err] = rtl::Read_File(handle, buffer);

		if (!count) {
			break;
		}

		// Cast new entry
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

// Struct of configuration
struct configuration {
	std::string file = ".";
	bool file_present = false;
	bool recurse = false;
	bool valid = true;
};

// Parse input arguments into configuration struct
configuration parse_args(const std::string args) {
	auto swt = utils::String_View_Tokenizer(args);
	auto result = configuration{};

	while (!swt.empty() && result.valid) {
		const auto token = swt();
		if (token == "/s" || token == "/S") {
			result.recurse = true;
		} else if (!result.file_present) {
			result.file = token;
			result.file_present = true;
		} else {
			result.valid = false;
		}
	}

	return result;
}

// Return absolute path of file or directory for output message
std::string get_output_string_path(std::string current_path, std::string file, bool isDir) {
	char file_type = isDir == true ? '+' : ' ';
	std::string delimiter = "\\";
	std::string path(file_type + current_path + delimiter + file);
	return path;
}

// Return absolute path of directory
std::string get_path(std::string current_path, std::string file) {
	std::string path(current_path + "\\" + file);
	return path;
}

// Recursive search through directories and save absolute path of files, directories and counts
size_t recursive_search(std::vector<kiv_os::TDir_Entry>& entries, std::vector<std::string>& all_entries, std::string path, kiv_os::THandle stdout_handle, size_t& f_count, size_t& d_count) {
	// Loop through entry in current directory
	for (const auto entry : entries) {
		// Store absolute path of file
		if (utils::is_file(static_cast<uint8_t>(entry.file_attributes))) {
			all_entries.push_back(get_output_string_path(path, entry.file_name, false));
			continue;
		}
		// Open directory file
		const auto [current_file_handle, error] = rtl::Open_File(get_path(path, entry.file_name), utils::get_dir_attrs());
		if (error != kiv_os::NOS_Error::Success) {
			rtl::Write_File(stdout_handle, utils::get_error_message(error));
			KIV_OS_EXIT(2);
		}
		// Store absolute path of directory
		all_entries.push_back(get_output_string_path(path, entry.file_name, true));
		auto sub_entries = std::vector<kiv_os::TDir_Entry>();
		// Load new entries of subdirectory
		const auto [file_count, dir_count] = load_entries(current_file_handle, sub_entries);
		f_count += file_count;
		d_count += dir_count;
		// Recursive call of subdirectory
		recursive_search(sub_entries, all_entries, get_path(path, entry.file_name), stdout_handle, f_count, d_count);
		rtl::Close_Handle(current_file_handle);
	}
	return 0;
}

// Command printing content of directory. If command contain "/S" parameter, print subdirectory recursively
size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	size_t f_count = 0;
	size_t d_count = 0;

	// Parse arguments
	auto args = std::string(reinterpret_cast<const char*>(regs.rdi.r));
	const auto config = parse_args(args);

	// Valid configuration
	if (!config.valid) {
		rtl::Write_File(stdout_handle, std::string("Invalid arguments\n"));
		KIV_OS_EXIT(2);
	}

	// Open file
	auto [file_handle, error] = rtl::Open_File(config.file, utils::get_dir_attrs());

	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	// Load entries
	auto entries = std::vector<kiv_os::TDir_Entry>();
	auto all_entries = std::vector<std::string>();
	auto [file_count, dir_count] = load_entries(file_handle, entries);
	f_count += file_count;
	d_count += dir_count;

	// If parameters contain "/S" print subdirectories recursively
	if (config.recurse) {
		std::string path;
		size_t chars_written = 0;
		kiv_os_rtl::Get_Working_Dir(path.data(), BUFFER_SIZE, chars_written);
		recursive_search(entries, all_entries, path, stdout_handle, f_count, d_count);
	}
	// Print only content of directory passed in parameters
	else {
		for (const auto entry : entries) {
			std::string file_type = utils::is_dir(static_cast<uint8_t>(entry.file_attributes)) ? "+" : " ";
			std::string entry_file(file_type + entry.file_name);
			all_entries.push_back(entry_file);
		}
	}

	// Print absolute paths of subdirectories and files
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
