#include "dir.h"
#include "utils.h"
#include <array>

std::pair<size_t, size_t> load_entries(const kiv_os::THandle handle, std::vector<kiv_os::TDir_Entry>& target) {
	auto buffer = std::array<char, dir_size>();
	auto files_counter = size_t(0);
	auto dir_counter = size_t(0);
	auto position = size_t(0);

	kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, position);
	target.reserve(16);
	while (true) {
		const auto [count, err] = kiv_os_rtl::Read_File(handle, buffer);

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
		return utils::is_dir(a.file_attributes) > utils::is_dir(b.file_attributes);
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
		return { false, results[0] };
	}
	
	if (results.size() == 2 && results[0] == "/S") {
		return { true, results[1] };
	}

	return { false, "." };
}

size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;

	// parse arguments
	auto args = std::string(reinterpret_cast<const char*>(regs.rdi.r));
	const auto [recurse, filename] = parse_args(args);

	// open file
	auto file_handle = kiv_os::THandle();
	auto error = kiv_os_rtl::Open_File(
		filename.data(), utils::get_dir_attrs(), kiv_os::NOpen_File::fmOpen_Always, file_handle
	);
	
	if (error != kiv_os::NOS_Error::Success) {
		kiv_os_rtl::Write_File(stdout_handle, utils::get_error_message(error));
		KIV_OS_EXIT(2);
	}

	// load entries
	auto entries = std::vector<kiv_os::TDir_Entry>();
	const auto [file_count, dir_count] = load_entries(file_handle, entries);

	// print
	std::ostringstream dir_content;
	dir_content
		<< "Directories: " << dir_count << new_line
		<< "Files: " << file_count << new_line;
	
	for (const auto entry : entries) {
		dir_content
			<< (utils::is_dir(entry.file_attributes) ? "+" : " ")
			<< entry.file_name
			<< new_line;
	}

	kiv_os_rtl::Write_File(stdout_handle, dir_content.str());
	kiv_os_rtl::Close_Handle(file_handle);

	KIV_OS_EXIT(0);
}
