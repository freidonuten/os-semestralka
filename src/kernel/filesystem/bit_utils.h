#pragma once

#include <cstdint>

class Bit_Utils {
private:

	static void Set_Individual_Bit_To_One(std::uint8_t* byte, std::uint8_t position_in_byte);
	static void Set_Individual_Bit_To_Zero(std::uint8_t* byte, std::uint8_t position_in_byte);
	static std::uint8_t Check_Individual_Bit_Of_Byte(std::uint8_t byte, std::uint8_t position_in_byte);

public:

	static void Set_Individual_Bit(void* data, std::uint64_t position, std::uint8_t value);
	static std::uint8_t Get_Individual_Bit(void* data, std::uint64_t position);

	static bool Check_Individual_Bit(std::uint8_t to_check, std::uint8_t mask);
};