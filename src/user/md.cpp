#include "md.h"

size_t __stdcall md(const kiv_hal::TRegisters& regs) {
	char* p_filename = reinterpret_cast<char*>(regs.rdx.r);
	std::string filename(p_filename);
	kiv_os::NOpen_File flag = static_cast<kiv_os::NOpen_File>(0);
	kiv_os::NFile_Attributes atributes = kiv_os::NFile_Attributes::Directory;
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os::THandle directory_handle;

	kiv_os_rtl::Open_File(filename, atributes, flag, directory_handle);
	kiv_os_rtl::Close_Handle(directory_handle);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}