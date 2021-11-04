#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class Bitmap {
private:
	std::uint8_t* data;
	std::uint64_t element_count;
	std::uint64_t size_in_bytes;

	std::uint64_t Get_Size_In_Bytes(std::uint64_t size_in_bits);
	std::unique_ptr<std::vector<std::uint64_t>> Insert_All_Set_Bits_To_Vector();
	void Set_All_Elements_To_Zero();
public:
	Bitmap(std::uint64_t element_count);
	~Bitmap();
	void Set_Element(std::uint64_t position);
	std::unique_ptr<std::vector<std::uint64_t>> Reset();
};