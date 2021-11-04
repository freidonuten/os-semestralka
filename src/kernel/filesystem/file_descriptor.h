#pragma once

#include <cstdint>
#include <memory>
#include <vector>

struct File_Descriptor {
	std::uint16_t descriptor_id;
	std::uint16_t fat_first_cluster;
	std::uint64_t file_size;
	std::uint64_t position;
};

class File_Descriptor_Table {
private:
	std::vector<std::shared_ptr<File_Descriptor>> descriptors;
	void Add_Descriptor(std::shared_ptr<File_Descriptor> descriptor);
public:
	File_Descriptor_Table();

	std::shared_ptr<File_Descriptor> Create_Descriptor(std::uint16_t fat_first_cluster);
	std::shared_ptr<File_Descriptor> Get_Descriptor(std::uint16_t id);
	void Remove_Handle(std::uint16_t id);


};