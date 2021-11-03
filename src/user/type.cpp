#include "type.h"

size_t __stdcall type(const kiv_hal::TRegisters& regs) {
	kiv_os::THandle file_handle;
	const kiv_os::THandle stdin_handle = regs.rax.x;
	const kiv_os::THandle stdout_handle = regs.rbx.x;
	char* p_filename = reinterpret_cast<char*>(regs.rdi.r);
	size_t chars_read = init_value;
	size_t chars_written = 0;
	size_t file_offset = 0;
	

	if (strlen(p_filename) == 0) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_INVALID_COMMAND_ARGUMENT.data(), ERROR_MSG_INVALID_COMMAND_ARGUMENT.size(), chars_written);
		kiv_os_rtl::Exit(1);
		return 1;
	}

	std::string filename(p_filename);
	std::string file_content = "";
	
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	kiv_os_rtl::Open_File(filename, kiv_os::NFile_Attributes::System_File, kiv_os::NOpen_File::fmOpen_Always, file_handle);

	// kontrola zda se podarilo otevrit soubor
	if (file_handle == invalid_file_handle) {
		kiv_os_rtl::Write_File(stdout_handle, ERROR_MSG_CANT_OPEN_FILE.data(), ERROR_MSG_CANT_OPEN_FILE.size(), chars_written);
		kiv_os_rtl::Exit(2);
		return 2;
	}

	while (chars_read == BUFFER_SIZE) {
		chars_read = 0;
		kiv_os_rtl::Seek(file_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, file_offset);
		kiv_os_rtl::Read_File(file_handle, buffer, BUFFER_SIZE, chars_read);
		file_content.append(buffer, chars_read);
		file_offset += chars_readed;
		memset(buffer, 0, BUFFER_SIZE);

	}

	kiv_os_rtl::Close_Handle(file_handle);
	kiv_os_rtl::Write_File(stdout_handle, file_content.data(), file_content.size(), chars_written);
	kiv_os_rtl::Exit(0);
	return 0;
}
