#include "rd.h"

size_t __stdcall rd(const kiv_hal::TRegisters& regs) {
	char* p_filename = reinterpret_cast<char *>(regs.rdx.r);
	std::string filename(p_filename);
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Delete_File(filename);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}
