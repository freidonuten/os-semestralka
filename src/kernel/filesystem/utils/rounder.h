#pragma once

#include <cstdint>
#include <memory>


class Rounder {
private:
	std::uint64_t limit;
	std::uint64_t Perform_Rounding_Up(std::uint64_t what_to_round_up, std::uint64_t divisor);
	bool Limit_Ok(std::uint64_t result);

public:
	Rounder(std::uint8_t size_of_int);
	std::uint64_t Round_Up(std::uint64_t what_to_round_up, std::uint64_t divisor);
};