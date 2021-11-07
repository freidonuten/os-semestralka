#pragma once

#include "fat_io.h"


#include <cstdint>
#include <memory>
#include <vector>


enum Fat12_Entry : std::uint16_t {
	FAT_EOF = 0x0FFE,
	FAT_FREE = 0x0FFF
};

class Fat12 {
private:
	std::unique_ptr<Fat_IO> fat_io;
	std::shared_ptr<Filesystem_Info> info;
	std::uint64_t last_allocated;

	std::uint16_t Allocate_One_Cluster();
	void Set_All_Entries_To_Free();
	std::uint64_t Get_Cluster_Count();
	std::uint16_t Get_Last_Cluster(std::uint16_t file_start);
public:

	Fat12(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::uint16_t Allocate_New_File();
	void Allocate_Clusters_For_Existing_File(std::uint16_t file_start, std::uint32_t how_many);
	std::vector<std::uint16_t> Get_File_Clusters(std::uint16_t file_start);
	
	void Deallocate_Last_N_Clusters(std::uint16_t file_start, std::uint32_t how_many);
	void Deallocate_File(std::uint16_t file_start);



};