#pragma once

#include "fat.h"
#include "cluster_io.h"
#include "global_structs.h"

#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>


class Fat_File {
private:
	std::uint16_t file_start;
	std::uint64_t file_size;
	std::uint32_t allocated_clusters;

	std::shared_ptr<Fat12> fat_table;
	std::shared_ptr<Cluster_IO> cluster_io;
	std::shared_ptr<Filesystem_Info> info;

	void Expand_File(std::uint64_t bytes_needed);
	void Change_Cluster_Size(std::uint64_t bytes_needed);
	int Calculate_Cluster_Difference(std::uint64_t desired_size);
	int Calculate_Cluster_Need(std::uint64_t size_in_bytes);
	std::vector<std::uint64_t> Cast_To_UInt64_Vector(std::vector<std::uint16_t> source);

public:
	Fat_File(std::shared_ptr<Fat12> fat_table, std::shared_ptr<Cluster_IO> cluster_io, std::shared_ptr<Filesystem_Info> info);
	Fat_File(std::shared_ptr<Fat12> fat_table, std::shared_ptr<Cluster_IO> cluster_io, std::shared_ptr<Filesystem_Info> info, std::uint16_t file_start, std::uint64_t file_size);

	std::uint16_t Get_File_Start();
	std::uint64_t Get_File_Size();

	void Create_New_File();
	void Write_To_File(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	void Read_From_File(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	void Change_File_Size(std::uint32_t desired_size);
	void Remove_File();


};

class Fat_File_Factory {
private:
	std::shared_ptr<Fat12> fat_table;
	std::shared_ptr<Cluster_IO> cluster_io;
	std::shared_ptr<Filesystem_Info> info;
public:
	Fat_File_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::shared_ptr<Fat_File> Create_New_File();
	std::shared_ptr<Fat_File> Get_Existing_File(std::uint16_t file_start, std::uint64_t file_size);
};

