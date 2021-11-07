#pragma once

#include <cstdint>
#include <memory>


class Endian_Converter  {
private:
	static std::uint8_t Get_Position(std::uint8_t byte, std::uint8_t bit);
	static std::uint8_t Get_Byte(std::uint8_t order);
	static std::uint8_t Get_Bit(std::uint8_t order);
public:
	//works both directions
	//little endiad => big endian
	//big endian => little endian
	static std::uint8_t Map(std::uint8_t position); 
};