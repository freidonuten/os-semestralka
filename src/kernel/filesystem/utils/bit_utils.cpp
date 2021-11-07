#include "bit_utils.h"

void Bit_Utils::Set_Individual_Bit(void* data, std::uint64_t bit, std::uint8_t value) {
	std::uint64_t byte_of_data = bit / 8;
	std::uint8_t bit_of_byte = bit % 8;

	if (value == 1) {
		Set_Individual_Bit_To_One(static_cast<std::uint8_t*>(data) + byte_of_data, bit_of_byte);
	}
	else if(value == 0) {
		Set_Individual_Bit_To_Zero(static_cast<std::uint8_t*>(data) + byte_of_data, bit_of_byte);
	}
}

std::uint8_t Bit_Utils::Get_Individual_Bit(void* data, std::uint64_t bit) {
	std::uint64_t byte_of_data = bit / 8;
	std::uint8_t bit_of_byte = bit % 8;

	return Check_Individual_Bit_Of_Byte(static_cast<std::uint8_t*>(data)[byte_of_data], bit_of_byte);
}

void Bit_Utils::Set_Individual_Bit_To_One(std::uint8_t* byte, std::uint8_t position_in_byte) {
	//   byte = 0000 0000
	//   set byte[5] to 1
	//   0000 0000 |= 0000 0001 << 2
	//=> 0000 0000 |= 0000 0100
	 *byte |= 1 << (7 - position_in_byte);
}


void Bit_Utils::Set_Individual_Bit_To_Zero(std::uint8_t* byte, std::uint8_t position_in_byte) {
	//   byte = 1111 1111
	//   set byte[5] to 0
	//   1111 1111 &= ~(0000 0001 << 2)
	//=> 1111 1111 &= 1111 1011
	*byte &= ~(1 << (7 - position_in_byte));
}


std::uint8_t Bit_Utils::Check_Individual_Bit_Of_Byte(std::uint8_t byte, std::uint8_t position_in_byte) {
	//byte = 1010 1010
	//get byte[5]
	//result = (1010 1010 >> 2) & (0000 0001)
	//result = 0010 1010 & 0000 0001
	return (byte >> (7 - position_in_byte)) & 1;
}


bool Bit_Utils::Check_Individual_Bit(std::uint8_t to_check, std::uint8_t mask) {
	std::uint8_t result = to_check & mask;
	if (result == 0) {
		return false;
	}
	else {
		return true;
	}

}