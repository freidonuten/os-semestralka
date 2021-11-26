#include "filesystem_info.h"
#include "../utils/rounder.h"
#include <memory>
#include <iostream>
#include <cmath>

Filesystem_Info::Filesystem_Info(std::uint64_t bytes_on_disk, std::uint8_t sectors_per_cluster,
	std::uint16_t bytes_per_sector, std::uint8_t fat_type) {
	this->bytes_per_data_blocks = new std::uint64_t[static_cast<int>(Data_Block::COUNT)];
	this->partitions_first_sectors = new std::uint64_t[static_cast<int>(Partition::COUNT)];

	//we will set sizes of DISK, CLUSTER and SECTOR, which are parameters of contructor
	Set_What_We_Know(bytes_on_disk, sectors_per_cluster, bytes_per_sector, fat_type);

	//the rest are sizes and starting sectors of FAT_TABLE and DATA_CLUSTERS (part of DISK, where DATA is)
	Calculate_The_Rest();
}

Filesystem_Info::~Filesystem_Info() {
	delete[] this->bytes_per_data_blocks;
	delete[] this->partitions_first_sectors;
}

void Filesystem_Info::Set_What_We_Know(std::uint64_t bytes_on_disk, std::uint8_t sectors_per_cluster,
	std::uint16_t bytes_per_sector, std::uint8_t fat_type) {
	

	this->bytes_per_data_blocks[static_cast<int>(Data_Block::DISK)] = bytes_on_disk;
	this->bytes_per_data_blocks[static_cast<int>(Data_Block::CLUSTER)] = sectors_per_cluster * bytes_per_sector;
	this->bytes_per_data_blocks[static_cast<int>(Data_Block::SECTOR)] = bytes_per_sector;
	this->fat_type = fat_type;
}

void Filesystem_Info::Calculate_The_Rest() {
	Calculate_Size_Of_Fat_Table();
	Calculate_Partitions();
	Calculate_Data_Clusters_Size();
}

void Filesystem_Info::Calculate_Size_Of_Fat_Table() {
	double raw_cluster_count = Calculate_Raw_Cluster_Count();
	double fat_entry_size = Calculate_Fat_Entry_Size();
	std::uint64_t fat_table_raw_size = static_cast<std::uint64_t>(std::ceil(raw_cluster_count * fat_entry_size));
	this->bytes_per_data_blocks[static_cast<int>(Data_Block::FAT_TABLE)] = Round_Up_Fat_Table_To_Fit_Sectors(fat_table_raw_size);
}

void Filesystem_Info::Calculate_Partitions() {
	std::uint64_t fat_table_size = Bytes_Per(Data_Block::FAT_TABLE);
	std::uint64_t sector_size = Bytes_Per(Data_Block::SECTOR);
	std::uint64_t fat_table_first_sector = 0;
	this->partitions_first_sectors[static_cast<int>(Partition::FAT_TABLE)] = fat_table_first_sector;
	this->partitions_first_sectors[static_cast<int>(Partition::DATA_CLUSTERS)] =
		fat_table_first_sector + (fat_table_size / sector_size); //100% divisible if rounding up correct
}

void Filesystem_Info::Calculate_Data_Clusters_Size() {
	std::uint64_t cluster_address_start = First_Sector_Of(Partition::DATA_CLUSTERS) * Bytes_Per(Data_Block::SECTOR);
	this->bytes_per_data_blocks[static_cast<int>(Data_Block::DATA_CLUSTERS)] = Bytes_Per(Data_Block::DISK) - cluster_address_start;
}

double Filesystem_Info::Calculate_Raw_Cluster_Count() {
	//    disk = (fat_type/8 * count) + (count *  cluster * sector) =>
	//=>  count = ( disk ) / ( fat_type/8 + cluster*sector )
	double right_parentheses = Calculate_Fat_Entry_Size() + Bytes_Per(Data_Block::CLUSTER);
	return static_cast<double>(Bytes_Per(Data_Block::DISK)) / right_parentheses;
}

double Filesystem_Info::Calculate_Fat_Entry_Size() {
	return static_cast<double>(fat_type) / 8;
}


std::uint64_t Filesystem_Info::Calculate_Bytes_Per_Cluster() {
	return Bytes_Per(Data_Block::CLUSTER);
}

std::uint64_t Filesystem_Info::Round_Up_Fat_Table_To_Fit_Sectors(std::uint64_t fat_table_raw_size) {
	auto rounder = std::make_unique<Rounder>(64);
	return rounder->Round_Up(fat_table_raw_size, Bytes_Per(Data_Block::SECTOR));
}



Address_To_Fat Filesystem_Info::Get_Absolute_Address_Of_Fat_Entry(std::uint64_t cluster) {
	Check_Cluster_Limit(cluster);
	return Calculate_Absolute_Address_Of_Fat_Entry(cluster);
}


void Filesystem_Info::Check_Cluster_Limit(std::uint64_t cluster) {
	if (cluster >= Get_Cluster_Count()) {
		throw std::overflow_error("Cluster out of bounds.");
	}
}

Address_To_Fat Filesystem_Info::Calculate_Absolute_Address_Of_Fat_Entry(std::uint64_t cluster) {
	Address_To_Fat result;

	std::uint64_t starting_bit = Calculate_First_Bit_Of_Fat_Entry(cluster);
	std::uint64_t bits_per_sector = Bytes_Per(Data_Block::SECTOR) * 8;

	result.sector = starting_bit / bits_per_sector;

	std::uint64_t starting_bit_in_sector = starting_bit % bits_per_sector;
	result.byte_address = starting_bit_in_sector / 8;
	result.bit_of_byte = starting_bit_in_sector % 8;

	return result;
}

std::uint64_t Filesystem_Info::Calculate_First_Bit_Of_Fat_Entry(std::uint64_t cluster) {
	std::uint64_t starting_bit_of_fat_table = First_Sector_Of(Partition::FAT_TABLE) * Bytes_Per(Data_Block::SECTOR) * 8;
	std::uint64_t starting_bit_of_entry_in_fat_table = cluster * this->fat_type;
	return starting_bit_of_fat_table + starting_bit_of_entry_in_fat_table;
}



std::uint64_t Filesystem_Info::Get_Cluster_Count() {
	std::uint64_t all_clusters_size = Bytes_Per(Data_Block::DATA_CLUSTERS);
	std::uint64_t one_cluster_size = Bytes_Per(Data_Block::CLUSTER);
	return all_clusters_size / one_cluster_size;
}



std::uint64_t Filesystem_Info::Bytes_Per(Data_Block type) {
	return this->bytes_per_data_blocks[static_cast<int>(type)];
}

std::uint64_t Filesystem_Info::First_Sector_Of(Partition partition) {
	return this->partitions_first_sectors[static_cast<int>(partition)];
}

std::uint8_t Filesystem_Info::Get_Fat_Type() {
	return this->fat_type;
}
