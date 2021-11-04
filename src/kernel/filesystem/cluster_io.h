#pragma once

#include "sector_io.h"
#include "filesystem_info.h"
#include "global_structs.h"

#include <memory>
#include <vector>

class Cluster_IO {
private:
	std::unique_ptr<Sector_IO> sector_io;
	std::shared_ptr<Filesystem_Info> info;

	std::vector<std::uint64_t> Get_Affected_Sectors_From_Clusters(std::vector<std::uint64_t> clusters_of_file, std::uint64_t start, size_t how_many);
	std::vector<std::uint64_t> Convert_Clusters_To_Sectors(std::vector<std::uint64_t> clusters);
	std::vector<std::uint64_t> Convert_Cluster_To_Sectors(std::vector<std::uint64_t> sectors, std::uint64_t cluster);

	std::uint64_t Calculate_First_Sector_Of_Data_Cluster(uint64_t cluster, std::uint64_t sectors_per_cluster);
	std::uint64_t Get_Sectors_Per_Cluster();

	std::vector<std::uint64_t> Get_Affected_Sectors_From_Sectors(std::vector<std::uint64_t> sectors_of_file, std::uint64_t start, size_t how_many);
	std::uint64_t Convert_Position_To_Sector_Index(std::uint64_t position);

	std::uint64_t Calculate_Start_In_Affected_Sectors(std::uint64_t start_in_clusters);
	

public:
	Cluster_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);
	
	void Proceed_Task(std::shared_ptr<IO_Task> task);
};