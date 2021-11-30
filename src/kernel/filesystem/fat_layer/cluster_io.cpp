#include "cluster_io.h"

Cluster_IO::Cluster_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->sector_io = std::make_unique<Sector_IO>(disk, info);
	this->info = info;
}

void Cluster_IO::Proceed_Task(std::shared_ptr<IO_Task> task) {
	auto clusters_of_file = task->data_blocks;
	auto start = task->starting_byte;
	auto how_many = task->how_many_bytes_to_operate;

	task->data_blocks = Get_Affected_Sectors_From_Clusters(clusters_of_file, start, how_many);
	task->starting_byte = Calculate_Start_In_Affected_Sectors(start);

	this->sector_io->Proceed_Task(task);
}


std::vector<std::uint64_t> Cluster_IO::Get_Affected_Sectors_From_Clusters(std::vector<std::uint64_t> clusters_of_file, std::uint64_t start, size_t how_many) {
	std::vector<std::uint64_t> sectors_of_file = Convert_Clusters_To_Sectors(clusters_of_file);
	return Get_Affected_Sectors_From_Sectors(sectors_of_file, start, how_many);
}

std::vector<std::uint64_t> Cluster_IO::Convert_Clusters_To_Sectors(std::vector<std::uint64_t> clusters) {
	std::vector<std::uint64_t> result;

	for (std::uint64_t cluster : clusters) {
		result = Convert_Cluster_To_Sectors(result, cluster);
	}

	return result;
}

std::vector<std::uint64_t> Cluster_IO::Convert_Cluster_To_Sectors(std::vector<std::uint64_t> sectors, std::uint64_t cluster) {
	std::uint64_t sectors_per_cluster = Get_Sectors_Per_Cluster();
	std::uint64_t first_sector = Calculate_First_Sector_Of_Data_Cluster(cluster, sectors_per_cluster);

	for (int i = 0; i < sectors_per_cluster; i++) {
		sectors.push_back(first_sector + i);
	}

	return sectors;
}

std::vector<std::uint64_t> Cluster_IO::Get_Affected_Sectors_From_Sectors(std::vector<std::uint64_t> sectors_of_file, std::uint64_t start, size_t how_many) {
	std::uint64_t end = start + how_many;
	std::uint64_t start_index = Convert_Position_To_Sector_Index(start);
	std::uint64_t end_index = Convert_Position_To_Sector_Index(end);

	std::vector<std::uint64_t> result;

	for (std::uint64_t i = start_index; i <= end_index; i++) {
		result.push_back(sectors_of_file[i]);
	}

	return result;
}

std::uint64_t Cluster_IO::Convert_Position_To_Sector_Index(std::uint64_t position) {
	if (position == 0) {
		return 0;
	}

	//1 -> 0
	//n -> 0
	//n + 1 -> 1
	//n = sector_bytes
	std::uint64_t a = position - 1;
	return a / this->info->Bytes_Per(Data_Block::SECTOR);
}


std::uint64_t Cluster_IO::Calculate_First_Sector_Of_Data_Cluster(std::uint64_t cluster, std::uint64_t sectors_per_cluster) {
	std::uint64_t sector_number_in_data = cluster * sectors_per_cluster;
	return this->info->First_Sector_Of(Partition::DATA_CLUSTERS) + sector_number_in_data;
}

std::uint64_t Cluster_IO::Get_Sectors_Per_Cluster() {
	std::uint64_t sector_size = this->info->Bytes_Per(Data_Block::SECTOR);
	std::uint64_t cluster_size = this->info->Bytes_Per(Data_Block::CLUSTER);
	return cluster_size / sector_size;
}

std::uint64_t Cluster_IO::Calculate_Start_In_Affected_Sectors(std::uint64_t start_in_clusters) {
	return start_in_clusters % this->info->Bytes_Per(Data_Block::SECTOR);
}