#include "stream.h"


std::uint64_t stream::Input_Console_Stream::Read(size_t n, void* buffer_vp) {
	auto buffer = static_cast<char*>(buffer_vp);
	auto registers = kiv_hal::TRegisters();
	auto pos = size_t(0);
	auto flush = false;

	while (pos < n) {
		//read char
		registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NKeyboard::Read_Char);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Keyboard, registers);
		
		if (!registers.flags.non_zero) {
			break;	//nic jsme neprecetli, 
		}			//pokud je rax.l EOT, pak byl zrejme vstup korektne ukoncen
					//jinak zrejme doslo k chybe zarizeni

		char ch = registers.rax.l;

		//osetrime zname kody
		switch (static_cast<kiv_hal::NControl_Codes>(ch)) {
			case kiv_hal::NControl_Codes::BS: {
					//mazeme znak z bufferu
					if (pos > 0) pos--;

					registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_Control_Char);
					registers.rdx.l = ch;
					kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
				}
				break;

			case kiv_hal::NControl_Codes::NUL:			//chyba cteni?
			case kiv_hal::NControl_Codes::EOT:			//konec textu
				buffer[pos++] = ch;
				return pos;
			case kiv_hal::NControl_Codes::LF:  break;	//jenom pohltime, ale necteme
			case kiv_hal::NControl_Codes::CR:
				ch = '\n';
				flush = true;


			default: buffer[pos++] = ch;
					 registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
					 registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(&ch);
					 registers.rcx.r = 1;
					 kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
					 if (flush) {
						 return pos;
					 }
					 break;
		}
	}

	return pos;
}

std::uint64_t stream::Output_Console_Stream::Write(size_t n, void* buffer) {
	kiv_hal::TRegisters registers;
	registers.rax.h = static_cast<decltype(registers.rax.h)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = reinterpret_cast<uint64_t>(buffer);
	registers.rcx.r = n;

	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);

	return registers.rax.r ? n : 0;
}

std::tuple<std::shared_ptr<VFS_Element>, std::shared_ptr<VFS_Element>>
stream::Factory() {
	return {
		std::make_shared<stream::Input_Console_Stream>(),
		std::make_shared<stream::Output_Console_Stream>(),
	};
}
