#include "seek_action.h"

void Seek_Action::Run() {
	std::uint16_t descriptor = this->registers->rdx.x;
	std::uint64_t seek_offset = this->registers->rdi.r;
	kiv_os::NFile_Seek seek_type = static_cast<kiv_os::NFile_Seek>(this->registers->rcx.l);
	kiv_os::NFile_Seek seek_operation = static_cast<kiv_os::NFile_Seek>(this->registers->rcx.h);

	auto table = this->vfs->Get_Descriptor_Table();
	auto entry = table->Get_Descriptor(descriptor);

	switch (seek_type) {
	case kiv_os::NFile_Seek::Beginning:
		entry->position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		entry->position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		auto dir_entry = entry->element->Generate_Dir_Entry();
		entry->position = dir_entry.file_size += seek_offset;
		break;
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		this->registers->rax.r = entry->position;
		break;
	case kiv_os::NFile_Seek::Set_Size:
		entry->element->Change_Size(entry->position);
		break;
	case kiv_os::NFile_Seek::Set_Position:
		//DO NOTHING
		break;
	}

}