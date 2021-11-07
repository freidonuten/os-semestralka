#include "fat_file.h"

Fat_File_Factory::Fat_File_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->info = info;
	this->cluster_io = std::make_shared<Cluster_IO>(disk, info);
	this->fat_table = std::make_shared<Fat12>(disk, info);

}

std::shared_ptr<Fat_File> Fat_File_Factory::Create_New_File() {
	std::shared_ptr<Fat_File> result = std::make_shared<Fat_File>(this->fat_table, this->cluster_io, this->info);
	result->Create_New_File();
	return result;
}

std::shared_ptr<Fat_File> Fat_File_Factory::Get_Existing_File(std::uint16_t file_start, std::uint64_t file_size) {
	return std::make_shared<Fat_File>(this->fat_table, this->cluster_io, this->info, file_start, file_size);
}

Fat_File::Fat_File(std::shared_ptr<Fat12> fat_table, std::shared_ptr<Cluster_IO> cluster_io, std::shared_ptr<Filesystem_Info> info) {
	this->fat_table = fat_table;
	this->cluster_io = cluster_io;
	this->info = info;

	this->file_size = static_cast<std::uint64_t>(-1);
	this->file_start = static_cast<std::uint16_t>(-1);
	this->allocated_clusters = 0;
}

Fat_File::Fat_File(std::shared_ptr<Fat12> fat_table, std::shared_ptr<Cluster_IO> cluster_io, std::shared_ptr<Filesystem_Info> info, std::uint16_t file_start, std::uint64_t file_size) {
	this->fat_table = fat_table;
	this->cluster_io = cluster_io;
	this->info = info;

	this->file_size = file_size;
	this->file_start = file_start;
	this->allocated_clusters = Calculate_Cluster_Need(this->file_size);
}


std::uint16_t Fat_File::Get_File_Start() {
	return this->file_start;
}

std::uint64_t Fat_File::Get_File_Size() {
	return this->file_size;
}


void Fat_File::Create_New_File() {
	this->file_start = this->fat_table->Allocate_New_File();
	this->file_size = 0;
	this->allocated_clusters = 1;
}

void Fat_File::Write_To_File(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	Expand_File(starting_byte + how_many_bytes);
	auto file_clusters = this->fat_table->Get_File_Clusters(file_start);
	auto data_blocks = Cast_To_UInt64_Vector(file_clusters);
	auto task = IO_Task_Factory::Make_Task(data_blocks, starting_byte, how_many_bytes, buffer, Task_Type::WRITE);
	this->cluster_io->Proceed_Task(task);

}

void Fat_File::Read_From_File(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	auto file_clusters = this->fat_table->Get_File_Clusters(file_start);
	auto data_blocks = Cast_To_UInt64_Vector(file_clusters);

	auto task = IO_Task_Factory::Make_Task(data_blocks, starting_byte, how_many_bytes, buffer, Task_Type::READ);
	this->cluster_io->Proceed_Task(task);
}

void Fat_File::Change_File_Size(std::uint32_t desired_size) {
	Change_Cluster_Size(desired_size);
	this->file_size = desired_size;
}

void Fat_File::Remove_File() {
	this->fat_table->Deallocate_File(file_start);
	this->file_start = static_cast<std::uint64_t>(-1);
	this->file_size = static_cast<std::uint64_t>(-1);
}


void Fat_File::Expand_File(std::uint64_t bytes_needed) {
	int clusters_to_allocate = Calculate_Cluster_Difference(bytes_needed);

	if (clusters_to_allocate > 0) {
		this->fat_table->Allocate_Clusters_For_Existing_File(file_start, clusters_to_allocate);
		this->allocated_clusters += clusters_to_allocate;
	}

	if (this->file_size < bytes_needed) {
		this->file_size = bytes_needed;
	}
}

void Fat_File::Change_Cluster_Size(std::uint64_t bytes_needed) {
	int cluster_difference = Calculate_Cluster_Difference(bytes_needed);

	if (cluster_difference > 0) {
		this->fat_table->Allocate_Clusters_For_Existing_File(file_start, cluster_difference);
	}
	else if (cluster_difference < 0) {
		this->fat_table->Deallocate_Last_N_Clusters(file_start, -1 * cluster_difference);
	}

	this->allocated_clusters += cluster_difference;
}

int Fat_File::Calculate_Cluster_Difference(std::uint64_t desired_size) {
	
	int current_cluster_count = static_cast<int>(this->allocated_clusters);
	int desired_cluter_count = Calculate_Cluster_Need(desired_size);
	return desired_cluter_count - current_cluster_count;
}

int Fat_File::Calculate_Cluster_Need(std::uint64_t size_in_bytes) {
	if (size_in_bytes == 0) {
		return 1;
	}

	std::uint64_t cluster_size = this->info->Bytes_Per(Data_Block::CLUSTER);
	double result = static_cast<double>(size_in_bytes) / cluster_size;
	result = std::ceil(result);
	return static_cast<int>(result);
}

std::vector<std::uint64_t> Fat_File::Cast_To_UInt64_Vector(std::vector<std::uint16_t> source) {
	std::vector<std::uint64_t> result;
	/*std::transform(source.begin(), source.end(), result.begin(),
		[](std::uint16_t old) {
			return static_cast<std::uint64_t>(old);
		});*/

	for (std::uint16_t source_value : source) {
		result.push_back(static_cast<std::uint64_t>(source_value));
	}
	return result;
}

