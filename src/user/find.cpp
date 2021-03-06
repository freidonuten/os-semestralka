#include "find.h"
#include "utils.h"
#include "rtl_wrappers.h"
#include <array>
#include "functional"

// Config structure
struct config {
	bool valid = true;
	bool count_only = false;
	bool inverse_search = false;
	bool pattern_written = false;
	bool input_written = false;
	std::string pattern = "";
	std::string input = "";
};

// Parse input arguments
config parse_arguments(const std::string_view arg_string) {
	auto swt = utils::String_View_Tokenizer(arg_string);
	auto result = config{};

	for (std::string_view arg; !swt.empty() && result.valid; ) {
		arg = swt();
		if (arg == "/v" || arg == "/V") {
			result.inverse_search = true;
		} else if (arg == "/c" || arg == "/C") {
			result.count_only = true;
		} else if (!result.pattern_written) {
			result.pattern = arg;
			result.pattern_written = true;
		} else if (!result.input_written) {
			result.input = arg;
			result.input_written = true;
		} else {
			result.valid = false;
		}
	}

	if (!result.pattern_written) {
		result.valid = false;
	}

	return result;
}


std::string filter(const std::string haystack, const std::string needle, const bool inverse, const bool count) {
	auto searcher = std::boyer_moore_searcher(needle.begin(), needle.end());
	auto ss = std::istringstream(haystack);
	auto output = std::ostringstream();
	auto matches = size_t(0);
	// this is to simulate the weird cmd.exe behaviour where "" matches nothing
	const auto empty_search = !needle.size();

	// apply pattern (needle) to each line
	for (std::string line; std::getline(ss, line, '\n'); ) {
		const auto match = std::search(line.begin(), line.end(), searcher) != line.end();
		if (inverse ^ match ^ empty_search) {
			// /c flag present, increase match count
			if (count) {
				++matches;
			// /c flag absent, write the line
			} else {
				output << line << new_line;
			}
		}
	}

	if (count) { // /c flag present, write the count
		output << matches << new_line;
	}

	return output.str();
}


std::string make_haystack(const kiv_os::THandle source_handle, const bool append_newlines) {
	auto contents = std::string("");
	auto buffer = std::array<char, BUFFER_SIZE>();

	// Load file/stream contents
	while (true) { 
		auto [count, eof, error] = rtl::Read_File(source_handle, buffer);
		if (eof) {
			break;
		}

		contents.append(buffer.data());
		if (append_newlines) {
			contents.append(new_line);
		}
	}

	return contents;
}

size_t __stdcall find(const kiv_hal::TRegisters& regs) {
	auto stdin_handle = kiv_os::THandle(regs.rax.x);
	auto stdout_handle = kiv_os::THandle(regs.rbx.x);
	auto file_handle = kiv_os::THandle(stdin_handle);
	auto error = kiv_os::NOS_Error::Success;

	// Parse arguments into config structure
	const auto config = parse_arguments(reinterpret_cast<char*>(regs.rdi.r));

	// Check if configuration is valid
	if (!config.valid) {
		// too many arguments or missing pattern
		KIV_OS_EXIT(1);
	}

	// Open file if specified
	if (config.input_written) { 
		std::tie(file_handle, error) = rtl::Open_File(config.input, utils::get_file_attrs());
		if (error != kiv_os::NOS_Error::Success) {
			rtl::Write_File(stdout_handle, utils::get_error_message(error));
			KIV_OS_EXIT(2);
		}
	}

	const auto haystack = make_haystack(file_handle, !config.input_written);

	// If reading from file, close it
	if (config.input_written) {
		rtl::Close_Handle(file_handle);
	}

	const auto result = filter(haystack, config.pattern, config.inverse_search, config.count_only);

	// Write to stdout
	rtl::Write_File(stdout_handle, result);

	KIV_OS_EXIT(0);
}
