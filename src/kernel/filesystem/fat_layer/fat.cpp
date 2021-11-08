#include "fat.h"

#include <iostream>

Fat12::Fat12(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->fat_io = std::make_unique<Fat_IO>(disk, info);
	this->info = info;
	this->last_allocated = 0;

	Set_All_Entries_To_Free();
}

void Fat12::Set_All_Entries_To_Free() {
	std::uint64_t cluster_count = Get_Cluster_Count();
	for (int i = 0; i < cluster_count; i++) {
		this->fat_io->Set_Entry_Value(i, FAT_FREE);
	}
	this->fat_io->Save_Changes();
}

std::uint64_t Fat12::Get_Cluster_Count() {
	std::uint64_t data_clusters_size = this->info->Bytes_Per(Data_Block::DATA_CLUSTERS);
	std::uint64_t one_cluster_size = this->info->Bytes_Per(Data_Block::CLUSTER);
	return data_clusters_size / one_cluster_size;
}

std::uint16_t Fat12::Allocate_New_File() {
	std::uint16_t new_cluster = Allocate_One_Cluster();
	this->fat_io->Set_Entry_Value(new_cluster, FAT_EOF);
	this->fat_io->Save_Changes();
	return new_cluster;
}


void Fat12::Allocate_Clusters_For_Existing_File(std::uint16_t file_start, std::uint32_t how_many) {
	std::uint16_t current_last = Get_Last_Cluster(file_start);
	for (int i = 0; i < how_many; i++) {
		std::uint16_t new_cluster = Allocate_One_Cluster();
		this->fat_io->Set_Entry_Value(current_last, new_cluster);
		current_last = new_cluster;
	}
	
	this->fat_io->Save_Changes();
}

void Fat12::Deallocate_Last_N_Clusters(std::uint16_t file_start, std::uint32_t how_many) {
	std::vector<std::uint16_t> clusters = Get_File_Clusters(file_start);
	std::uint16_t file_cluster_count = clusters.size();

	for (int i = 0; i < how_many; i++) {
		std::uint16_t position = file_cluster_count - how_many + i;
		this->fat_io->Set_Entry_Value(position, FAT_FREE);
	}

	std::uint16_t last_not_deallocated = file_cluster_count - how_many - 1;
	this->fat_io->Set_Entry_Value(last_not_deallocated, FAT_EOF);
	this->fat_io->Save_Changes();
}

void Fat12::Deallocate_File(std::uint16_t file_start) {
	std::vector<std::uint16_t> clusters = Get_File_Clusters(file_start);
	std::uint16_t file_cluster_count = clusters.size();

	for (int i = 0; i < file_cluster_count; i++) {
		this->fat_io->Set_Entry_Value(i, FAT_FREE);
	}

	this->fat_io->Save_Changes();
}

std::uint16_t Fat12::Get_Last_Cluster(std::uint16_t file_start) {
	std::vector<std::uint16_t> clusters = Get_File_Clusters(file_start);
	return clusters.back();
}


std::vector<std::uint16_t> Fat12::Get_File_Clusters(std::uint16_t file_start) {
	std::vector<std::uint16_t> result;
	result.push_back(file_start);

	std::uint16_t current = file_start;
	while (true) {
		std::uint16_t next = this->fat_io->Get_Entry_Value(current);
		if (next == FAT_EOF) {
			break;
		}
		result.push_back(next);
		current = next;
	}

	return result;
}




std::uint16_t Fat12::Allocate_One_Cluster() {
	std::uint64_t cluster_count = Get_Cluster_Count();

	for (int i = 0; i < cluster_count; i++) {
		std::uint64_t position = (this->last_allocated + i) % cluster_count;
		std::uint16_t val = this->fat_io->Get_Entry_Value(position);

		if (val == FAT_FREE) {
			this->fat_io->Set_Entry_Value(position, FAT_EOF);
			last_allocated = position;
			return position;
		}
	}

	//TODO don't found any free
	std::cout << "Can't find any free clusters" << std::endl;
	return 0xFFFF;
}
