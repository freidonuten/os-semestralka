#include "dir.h"

size_t __stdcall dir(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	const char* p_filename = reinterpret_cast<const char*>(regs.rdi.r);
	std::vector<kiv_os::TDir_Entry*> entries;
	size_t chars_read = 0;
	size_t chars_written = 0;
	size_t offset = 0;
	size_t files_counter = 0;
	size_t dir_counter = 0;
	char buffer[dir_size];
	 
	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}
	
	std::string filename(p_filename);
	kiv_os_rtl::Open_File(filename, kiv_os::NFile_Attributes::Directory, kiv_os::NOpen_File::fmOpen_Always, file_handle);
	
	if (file_handle == invalid_file_handle) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	while (true) {
		memset(buffer, 0, dir_size);
		kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, offset);
		kiv_os_rtl::Read_File(file_handle, buffer, dir_size, chars_read);
		if (chars_read == 0) {
			break;
		}
		kiv_os::TDir_Entry* entry = reinterpret_cast<kiv_os::TDir_Entry*>(buffer);

		if (entry->file_attributes == dir_type) {
			dir_counter++;
		} else if (entry->file_attributes == file_type) {
			files_counter++;
		}
		entries.push_back(entry);
		offset += chars_read;
	}

	std::ostringstream dir_content;
	for (const auto entry : entries) {
		dir_content << entry->file_name << new_line;
	}

	dir_content << new_line << "Files: " << files_counter << new_line << "Directories: " << dir_counter << new_line;
	kiv_os_rtl::Write_File(stdout_handle, dir_content.str().data(), dir_content.str().size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}