#pragma once

#include "filesystem_info.h"
#include "disk.h"

#include <cstdint>
#include <vector>

enum class Fat12_Entry : std::uint16_t {
	FAT_EOF = 0x0FFF,
	FAT_FREE = 0x0FFE
};

class Fat12_Table {
private:

	std::shared_ptr<Filesystem_Info> calculator;
	std::shared_ptr<IDisk> disk;
	std::vector<Fat12_Entry> data;

	std::vector<std::uint64_t> sectors_to_update;

	Fat12_Entry Get_Value(std::uint16_t cluster_index);
	void Set_Value(std::uint16_t cluster_index, Fat12_Entry value);

	std::uint64_t Get_Cluster_Count();
	



public:

	Fat12_Table(std::shared_ptr<Filesystem_Info> calculator, std::shared_ptr<IDisk> disk);


};