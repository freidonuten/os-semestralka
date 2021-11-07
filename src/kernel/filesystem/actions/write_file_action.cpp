#include "write_file_action.h"

void Write_File_Action::Run() {
	std::uint16_t descriptor = this->registers->rdx.x;
	void* buffer = reinterpret_cast<char*>(this->registers->rdi.r);
	std::uint64_t how_many_bytes = this->registers->rcx.r;

	auto table = this->vfs->Get_Descriptor_Table();
	auto entry = table->Get_Descriptor(descriptor);

	std::uint64_t written_bytes = entry->element->Write(entry->position, how_many_bytes, buffer);
	entry->position += written_bytes;

	this->registers->rax.r = written_bytes;
}