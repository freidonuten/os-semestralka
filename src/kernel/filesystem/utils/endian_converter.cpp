#include "endian_converter.h"

std::uint8_t Endian_Converter::Get_Position(std::uint8_t byte, std::uint8_t bit) {
	return 8 * byte + bit;
}

std::uint8_t Endian_Converter::Get_Byte(std::uint8_t position) {
	return position / 8;
}

std::uint8_t Endian_Converter::Get_Bit(std::uint8_t position) {
	return position % 8;
}

std::uint8_t Endian_Converter::Map(std::uint8_t position) {
	std::uint8_t output_byte = 7 - Get_Byte(position);
	std::uint8_t output_bit = Get_Bit(position);
	return Get_Position(output_byte, output_bit);
}