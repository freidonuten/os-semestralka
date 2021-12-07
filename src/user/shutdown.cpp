#include "shutdown.h"
#include "rtl_wrappers.h"

size_t __stdcall shutdown(const kiv_hal::TRegisters& regs) {
	rtl::Shutdown();
	return 0;
}
