#include "dir.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>
#include <deque>

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
		target.push_back(entry);
	}

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

/* This is just a simple wildcard matching algorithm, supports both * and ? */
bool wildcard_matcher(const std::string_view text, const std::string_view pattern) {
	const auto t_size = text.size();
	const auto p_size = pattern.size();

	if (!p_size) {
		return !t_size;
	}

	auto i = long(0);
	auto j = long(0);

	const auto pattern_at = [pattern, &j, p_size](const auto c) {
		return (j < p_size) && (pattern[j] == c);
	};

	for (long pi = -1, ti = -1; i < t_size; ) {
		if (pattern_at(text[i]) || pattern_at('?')) {
			++i; ++j;
		}
		else if (pattern_at('*')) {
			ti = i;
			pi = j++;
		}
		else if (pi != -1) {
			i = ++ti;
			j = pi + 1;
		}
		else {
			return false;
		}
	}

	while (pattern_at('*')) {
		++j;
	}

	return j == p_size;
}

/*
 * Splits path to base and wildcard pattern
 * - base is unambiguous and can be querried as it is
 * - pattern is the rest of path that is going to be matched agains base contents
 *   pattern can be deeper than 1, in conjuction with /s the whole subtree is going to be matched
 */
std::pair<std::string, std::string> wildcard_path_split(const std::string_view path) {
	const auto glob_start = path.find_first_of("*?");
	auto last_slash = size_t(0);

	if (glob_start == std::string_view::npos) {
		return { std::string(path), std::string() };
	}

	for (auto i = glob_start; i != 0; --i) {
		if (path[i] == '\\') {
			last_slash = i;
			break;
		}
	}

	const auto split = last_slash > 0 ? last_slash + 1 : 0;

	return {
		!split ? "." : std::string(path.data(), split),
		std::string(path.data() + split, path.size() - split)
	};
}

// Return absolute path of directory
std::string get_path(std::string current_path, std::string file) {
	std::string path(current_path + (current_path[current_path.size() - 1] == '\\' ? "" : "\\") + file);
	return path;
}

/*
 * Iterative path aware directory tree recursion.
 */
void path_search(const std::vector<kiv_os::TDir_Entry> base, const std::string path, std::vector<std::pair<kiv_os::TDir_Entry, std::string>>& files) {
	auto stack = std::deque<std::pair<kiv_os::TDir_Entry, std::string>>{};
	auto buffer = std::array<char, dir_size>{};

	std::for_each(base.begin(), base.end(), [&stack, &path](const auto entry) {
		stack.push_back({ entry, path }); // push entries onto the stack
	});

	while (stack.size()) { // this is just plain old DFS
		const auto [ entry, path ] = stack.back();

		stack.pop_back();
		files.push_back({ entry, path });

		if (utils::is_file(static_cast<uint8_t>(entry.file_attributes))) {
			continue; // don't recurse plain files
		}

		const auto new_path = get_path(path, entry.file_name);
		const auto [handle, err] = rtl::Open_File(new_path, utils::get_dir_attrs());
		auto pos = size_t(0);
		kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, pos);

		while (1) { // recurse directory
			const auto [count, eof, err] = rtl::Read_File(handle, buffer);
			if (!count) {
				break;
			}
			const auto child_entry = *(reinterpret_cast<kiv_os::TDir_Entry*>(buffer.data()));
			stack.push_back({ child_entry, new_path });
		}

		rtl::Close_Handle(handle);
	}
}

// Command printing content of directory. If command contain "/S" parameter, print subdirectory recursively
size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;

	// Parse arguments
	auto args = std::string(reinterpret_cast<const char*>(regs.rdi.r));
	const auto config = parse_args(args);

	// Valid configuration
	if (!config.valid) {
		rtl::Write_File(stdout_handle, std::string("Invalid arguments\n"));
		KIV_OS_EXIT(2);
	}

	const auto [path_base, path_glob] = wildcard_path_split(config.file);
	const auto wildcard_matching = path_base.size() != config.file.size();

	// open file
	auto [file_handle, error] = rtl::Open_File(path_base, utils::get_dir_attrs());
	if (error != kiv_os::NOS_Error::Success) {
		rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	// load entries
	auto base_entries = std::vector<kiv_os::TDir_Entry>();
	auto all_entries = std::vector<std::pair<kiv_os::TDir_Entry, std::string>>{};
	auto [file_count, dir_count] = load_entries(file_handle, base_entries);

	rtl::Close_Handle(file_handle);

	// perform recursion or simply map entries to paths
	if (config.recurse) {
		path_search(base_entries, path_base, all_entries);
	} else {
		const auto path = path_base;
		std::for_each(base_entries.begin(), base_entries.end(), [&all_entries, &path](const auto entry) {
			all_entries.push_back({ entry, path });
		});
	}

	// prepare output string
	// arrays are addressed by boolean: false = files, true = dirs
	constexpr auto file_flag = std::array<char, 2>{ ' ', '+' };
	auto counters = std::array<size_t, 2>{ 0 }; 
	auto output = std::ostringstream{ };

	for (const auto [ entry, path ] : all_entries) {
		// remove querried path prefix: dir a/b/ matches a/b/c which should output as c
		const auto path_join = get_path(path, entry.file_name).substr(path_base.size());
		if (!wildcard_matching || wildcard_matcher(path_join, path_glob)) {
			const auto is_dir = utils::is_dir(static_cast<uint8_t>(entry.file_attributes));
			counters[is_dir]++;
			output
				<< file_flag[is_dir]
				<< std::string_view(path_join.data() + (path_join[0] == '\\'))
				<< new_line;
		}
	}

	output
		<< '\t' << counters[1] << " Dir(s)\n"
		<< '\t' << counters[0] << " File(s)\n";

	// flush output to stdout
	rtl::Write_File(stdout_handle, output.str());

	KIV_OS_EXIT(0);
}
