#include "bitmap.h"
#include "rounder.h"
#include "bit_utils.h"

#include <memory>

Bitmap::Bitmap(std::uint64_t element_count) {
	std::uint64_t size_in_bytes = Get_Size_In_Bytes(element_count);
	this->size_in_bytes = size_in_bytes;
	this->element_count = element_count;

	this->data = new std::uint8_t[size_in_bytes];
	for (int i = 0; i < size_in_bytes; i++) {
		this->data[i] = 0;
	}
}

Bitmap::~Bitmap() {
	delete[] this->data;
}

std::uint64_t Bitmap::Get_Size_In_Bytes(std::uint64_t size_in_bits) {
	auto rounder = std::make_unique<Rounder>(64);
	std::uint64_t rounded_size = rounder->Round_Up(size_in_bits, 8);
	return rounded_size / 8;
}

void Bitmap::Set_Element(std::uint64_t position) {
	void* void_data = static_cast<void*>(this->data);
	Bit_Utils::Set_Individual_Bit(void_data, position, 1);
}

std::unique_ptr<std::vector<std::uint64_t>> Bitmap::Reset() {
	auto result = Insert_All_Set_Bits_To_Vector();
	Set_All_Elements_To_Zero();
	return std::move(result);
}

std::unique_ptr<std::vector<std::uint64_t>> Bitmap::Insert_All_Set_Bits_To_Vector() {
	auto result = std::make_unique<std::vector<std::uint64_t>>();
	void* void_data = static_cast<void*>(this->data);

	for (int i = 0; i < this->element_count; i++) {
		std::uint8_t current_bit = Bit_Utils::Get_Individual_Bit(void_data, i);

		if (current_bit == 1) {
			result->push_back(i);
		}
	}

	return std::move(result);
}

void Bitmap::Set_All_Elements_To_Zero() {
	for (int i = 0; i < this->size_in_bytes; i++) {
		this->data[i] = 0;
	}
}