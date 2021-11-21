#pragma once

#include "../utils/global_structs.h"
#include "../../../api/api.h"
#include <cstdint>

class VFS_Element2 {
public:
	virtual void Open(std::uint16_t file_start, std::uint16_t file_size);
	virtual void Create();
	virtual bool Remove();
	virtual void Close();
	virtual bool VFS_Element2::Set_File_Attributes(std::uint16_t file_attributes);

	virtual std::uint64_t Write(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Read(size_t how_many_bytes, void* buffer);
	virtual std::uint64_t Seek(std::uint64_t offset, kiv_os::NFile_Seek start_position, kiv_os::NFile_Seek seek_operation);

	virtual Fat_Dir_Entry Generate_Dir_Entry();




};