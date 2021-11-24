#include "vfs_element.h"
#include <stdexcept>


void VFS_Element2::Open(std::uint16_t file_start, std::uint16_t file_size) {
	throw std::runtime_error("Unsupported operation OPEN.");
}

void VFS_Element2::Create() {
	throw std::runtime_error("Unsupported operation CREATE.");
}

bool VFS_Element2::Remove() {
	throw std::runtime_error("Unsupported operation OPEN.");
}

void VFS_Element2::Close() {
	throw std::runtime_error("Unsupported operation CLOSE.");
}

bool VFS_Element2::Set_File_Attributes(std::uint16_t file_attributes) {
	throw std::runtime_error("Unsupported operation SET FILE ATTRIBUTES.");
}

std::uint64_t VFS_Element2::Write(size_t how_many_bytes, void* buffer) {
	throw std::runtime_error("Unsupported operation WRITE.");
}

std::uint64_t VFS_Element2::Read(size_t how_many_bytes, void* buffer) {
	throw std::runtime_error("Unsupported operation READ.");
}

std::tuple<uint64_t, Seek_Result> VFS_Element2::Seek(std::uint64_t offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation) {
	throw std::runtime_error("Unsupported operation SEEK.");
}

Fat_Dir_Entry VFS_Element2::Generate_Dir_Entry() {
	throw std::runtime_error("Unsupported operation GENERATE DIR ENTRY.");
}


