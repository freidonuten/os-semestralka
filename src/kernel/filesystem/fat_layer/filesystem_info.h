#pragma once

#include <cstdint>

enum class Partition : std::uint8_t { 
	FAT_TABLE = 0,
	DATA_CLUSTERS,
	COUNT //for array creation
};

enum class Data_Block : std::uint8_t {
	SECTOR = 0,
	CLUSTER,
	FAT_TABLE,
	DATA_CLUSTERS,
	DISK,
	COUNT //for array creation
};

struct Address_To_Fat {
	std::uint64_t sector;
	std::uint64_t byte_address;
	std::uint8_t bit_of_byte;
};

class Filesystem_Info {
private:
	std::uint8_t fat_type; //FAT12, FAT16, FAT32 etc.

	std::uint64_t* partitions_first_sectors;
	std::uint64_t* bytes_per_data_blocks;

	void Set_What_We_Know(std::uint64_t bytes_on_disk, std::uint8_t sectors_per_cluster,
		std::uint16_t bytes_per_sector, std::uint8_t fat_type);
	void Calculate_The_Rest();
	void Calculate_Partitions();
	void Calculate_Size_Of_Fat_Table();
	double Calculate_Raw_Cluster_Count();
	double Calculate_Fat_Entry_Size();
	std::uint64_t Calculate_Bytes_Per_Cluster();
	std::uint64_t Round_Up_Fat_Table_To_Fit_Sectors(std::uint64_t fat_table_raw_size);
	void Calculate_Data_Clusters_Size();

	void Check_Cluster_Limit(std::uint64_t cluster);
	Address_To_Fat Calculate_Absolute_Address_Of_Fat_Entry(std::uint64_t cluster);
	std::uint64_t Calculate_First_Bit_Of_Fat_Entry(std::uint64_t cluster);
	std::uint64_t Get_Cluster_Count();
	

public:
	Filesystem_Info(std::uint64_t bytes_on_disk, std::uint8_t sectors_per_cluster, std::uint16_t bytes_per_sector, std::uint8_t fat_type);
	~Filesystem_Info();

	Address_To_Fat Get_Absolute_Address_Of_Fat_Entry(uint64_t cluster);
	

	std::uint64_t Bytes_Per(Data_Block type);
	std::uint64_t First_Sector_Of(Partition partition);
	std::uint8_t Get_Fat_Type();
	
};