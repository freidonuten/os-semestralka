#pragma once

#include "filesystem_info.h"
#include "disk.h"
#include "../utils/bitmap.h"


#include <cstdint>
#include <memory>

class Fat_IO {
private:
	std::shared_ptr<Filesystem_Info> info;
	std::shared_ptr<IDisk> disk;
	std::unique_ptr<Bitmap> bitmap_for_saving;
	std::uint8_t* fat_table_in_ram;

	std::uint64_t Get_Number_Of_Sectors();
	std::uint64_t Get_Size_Of_Fat_Table();

	std::uint64_t Get_First_Bit_Of_Fat_Entry(std::uint64_t cluster);
	std::uint64_t Get_Sector_From_Bit(std::uint64_t bit);

	void Set_Sectors_In_Bitmap(std::uint64_t fat_starting_bit);
	void Copy_Bits_To_Fat(std::uint64_t fat_starting_bit, std::uint64_t value);
	std::uint64_t Copy_Bits_From_Fat(std::uint64_t starting_bit_of_fat);
	void Update_Sector(std::uint64_t relative_order_of_sector);
	void* Get_Data_Of_Sector(std::uint64_t relative_order_of_sector);


public:
	Fat_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);
	~Fat_IO();

	void Set_Entry_Value(std::uint16_t position, std::uint16_t value);
	std::uint16_t Get_Entry_Value(std::uint16_t position);
	void Save_Changes();
};