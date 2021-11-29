#include "dir.h"
#include "utils.h"

size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	const char* p_filename = reinterpret_cast<const char*>(regs.rdi.r);
	std::vector<kiv_os::TDir_Entry> entries;
	size_t chars_read = 0;
	size_t chars_written = 0;
	size_t offset = 0;
	size_t files_counter = 0;
	size_t dir_counter = 0;
	char buffer[dir_size];
	std::string empty_file_name = ".";
	kiv_os::NOS_Error error;
	 
	if (strlen(p_filename) == 0) {
		p_filename = empty_file_name.c_str();
	}
	
	std::string filename(p_filename);
	uint8_t file_attrs = utils::get_dir_attrs();
	error = kiv_os_rtl::Open_File(filename, file_attrs, kiv_os::NOpen_File::fmOpen_Always, file_handle);
	
	if (error != kiv_os::NOS_Error::Success) {
		auto message = utils::get_error_message(error);
		kiv_os_rtl::Write_File(stdout_handle, message.data(), message.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, offset);
	while (true) {
		memset(buffer, 0, dir_size);
		
		kiv_os_rtl::Read_File(file_handle, buffer, dir_size, chars_read);
		if (chars_read == 0) {
			break;
		}
		kiv_os::TDir_Entry entry = *(reinterpret_cast<kiv_os::TDir_Entry*>(buffer));

		if (utils::is_dir(entry.file_attributes)) {
			dir_counter++;
		}
		else {
			files_counter++;
		}

		entries.push_back(entry);
	}

	std::ostringstream dir_content;
	dir_content << "Files: " << files_counter << new_line << "Directories: " << dir_counter << new_line;

	
	for (const auto entry : entries) {
		dir_content << entry.file_name << new_line;
	}

	kiv_os_rtl::Write_File(stdout_handle, dir_content.str().data(), dir_content.str().size(), chars_written);
	kiv_os_rtl::Close_Handle(file_handle);
	kiv_os_rtl::Exit(0);
	return 0;
}
