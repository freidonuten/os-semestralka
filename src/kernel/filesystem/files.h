#pragma once

#include "fat.h"
#include "cluster_io.h"
#include "io_task.h"

#include <cstdint>
#include <memory>
#include <vector>

class Files {
private:
	std::shared_ptr<Fat12> fat_table;
	std::shared_ptr<Cluster_IO> cluster_io;
	std::shared_ptr<Filesystem_Info> info;

	int Calculate_Cluster_Difference(std::vector<std::uint16_t> file_clusters, std::uint64_t bytes_count);
	std::uint32_t How_Many_Clusters_To_Allocate(std::vector<std::uint16_t> file_clusters, std::uint64_t bytes_needed);
	void Expand_File(std::uint64_t file_start, std::vector<std::uint16_t> file_clusters, std::uint64_t bytes_needed);
	std::vector<std::uint64_t> Cast_To_UInt64_Vector(std::vector<std::uint16_t> source);
public:
	Files(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	std::uint16_t Create_New_File();
	void Write_To_File(std::uint64_t file_start, std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	void Read_From_File(std::uint64_t file_start, std::uint64_t starting_byte, size_t how_many_bytes, void* buffer);
	void Change_File_Size(std::uint64_t file_start, std::uint32_t desired_size);
	void Remove_File(std::uint64_t file_start);
};