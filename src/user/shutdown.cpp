#include "shutdown.h"

size_t __stdcall shutdown(const kiv_hal::TRegisters& regs) {
	uint16_t exit_code = 0;
	kiv_os_rtl::Shutdown();
	kiv_os_rtl::Exit(exit_code);
	return size_t();
}
