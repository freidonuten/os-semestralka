#include "fat_io.h"
#include "endian_converter.h"
#include "bit_utils.h"

Fat_IO::Fat_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->disk = disk;
	this->info = info;
	
	std::uint64_t bitmap_size = Get_Number_Of_Sectors();
	this->bitmap_for_saving = std::make_unique<Bitmap>(bitmap_size);

	std::uint64_t fat_table_size = Get_Size_Of_Fat_Table();
	this->fat_table_in_ram = new std::uint8_t[fat_table_size];
}

Fat_IO::~Fat_IO() {
	delete[] this->fat_table_in_ram;
}

std::uint64_t Fat_IO::Get_Number_Of_Sectors() {
	std::uint64_t disk_size = this->info->Bytes_Per(Data_Block::DISK);
	std::uint64_t sector_size = this->info->Bytes_Per(Data_Block::SECTOR);
	return disk_size / sector_size;
}

std::uint64_t Fat_IO::Get_Size_Of_Fat_Table() {
	return this->info->Bytes_Per(Data_Block::FAT_TABLE);
}

void Fat_IO::Set_Entry_Value(std::uint64_t position, std::uint64_t value) {
	std::uint64_t first_bit_of_entry = Get_First_Bit_Of_Fat_Entry(position);
	Set_Sectors_In_Bitmap(first_bit_of_entry);
	Copy_Bits_To_Fat(first_bit_of_entry, value);
}


std::uint64_t Fat_IO::Get_First_Bit_Of_Fat_Entry(std::uint64_t cluster) {
	std::uint8_t fat_type = this->info->Get_Fat_Type();
	return cluster * fat_type;
}


std::uint64_t Fat_IO::Get_Sector_From_Bit(std::uint64_t bit) {
	return (bit / 8) / this->info->Bytes_Per(Data_Block::SECTOR);
}

void Fat_IO::Set_Sectors_In_Bitmap(std::uint64_t fat_starting_bit) {
	std::uint64_t fat_ending_bit = fat_starting_bit + this->info->Get_Fat_Type() - 1;
	std::uint64_t fat_starting_sector = Get_Sector_From_Bit(fat_starting_bit);
	std::uint64_t fat_ending_sector = Get_Sector_From_Bit(fat_ending_bit);

	for (int i = fat_starting_sector; i <= fat_ending_sector; i++) {
		this->bitmap_for_saving->Set_Element(i);
	}
}

void Fat_IO::Copy_Bits_To_Fat(std::uint64_t fat_starting_bit, std::uint64_t value) {
	std::uint8_t fat_type = this->info->Get_Fat_Type();
	void* input_data = static_cast<void*>(&value);
	void* output_data = static_cast<void*>(this->fat_table_in_ram);

	for (int i = 0; i < fat_type; i++) {
		std::uint8_t big_endian_position = (64 - fat_type) + i;
		std::uint8_t little_endian_position = Endian_Converter::Map(big_endian_position);
		std::uint8_t bit = Bit_Utils::Get_Individual_Bit(input_data, little_endian_position);
		Bit_Utils::Set_Individual_Bit(output_data, fat_starting_bit + i, bit);
	}
}


std::uint64_t Fat_IO::Get_Entry_Value(std::uint64_t position) {
	std::uint64_t entry_first_bit = Get_First_Bit_Of_Fat_Entry(position);
	return Copy_Bits_From_Fat(entry_first_bit);
}

std::uint64_t Fat_IO::Copy_Bits_From_Fat(std::uint64_t fat_starting_bit) {
	std::uint64_t result = 0;

	std::uint8_t fat_type = this->info->Get_Fat_Type();
	void* input_data = static_cast<void*>(this->fat_table_in_ram);
	void* output_data = static_cast<void*>(&result);

	for (int i = 0; i < fat_type; i++) {
		std::uint8_t bit = Bit_Utils::Get_Individual_Bit(input_data, fat_starting_bit + i);
		std::uint8_t big_endian_position = (64 - fat_type) + i;
		std::uint8_t little_endian_position = Endian_Converter::Map(big_endian_position);
		Bit_Utils::Set_Individual_Bit(output_data, little_endian_position, bit);
	}

	return result;
}

void Fat_IO::Save_Changes() {
	auto vector_ptr = this->bitmap_for_saving->Reset();

	for (auto vector_val : *vector_ptr) {
		Update_Sector(vector_val);
	}
}

void Fat_IO::Update_Sector(std::uint64_t relative_order_of_sector) {
	void* data = Get_Data_Of_Sector(relative_order_of_sector);
	std::uint64_t absolute_order_of_sector = relative_order_of_sector + this->info->First_Sector_Of(Partition::FAT_TABLE);
	this->disk->Write(1, data, absolute_order_of_sector);
}

void* Fat_IO::Get_Data_Of_Sector(std::uint64_t relative_order_of_sector) {
	std::uint64_t bytes_per_sector = this->info->Bytes_Per(Data_Block::SECTOR);
	std::uint64_t position = relative_order_of_sector * bytes_per_sector;
	return static_cast<void*>(this->fat_table_in_ram + position);
}

