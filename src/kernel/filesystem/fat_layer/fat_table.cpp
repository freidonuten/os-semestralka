#include "fat_table.h"
#include "../utils/rounder.h"

Fat12_Table::Fat12_Table(std::shared_ptr<Filesystem_Info> calculator, std::shared_ptr<IDisk> disk) {
	this->calculator = calculator;
	this->disk = disk;
	std::uint64_t cluster_count = Get_Cluster_Count();
	this->data = std::vector<Fat12_Entry>(cluster_count);
	this->sectors_to_update = std::vector<std::uint64_t>();
	
}

std::uint64_t Fat12_Table::Get_Cluster_Count() {
	std::uint64_t all_clusters_size = this->calculator->Bytes_Per(Data_Block::DATA_CLUSTERS);
	std::uint64_t one_cluster_size = this->calculator->Bytes_Per(Data_Block::CLUSTER);
	return all_clusters_size / one_cluster_size;
}


Fat12_Entry Fat12_Table::Get_Value(std::uint16_t cluster_index) {
	return this->data[cluster_index];
}


