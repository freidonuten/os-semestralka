#include <cmath>
#include <stdexcept>
#include <limits>
#include "rounder.h"


Rounder::Rounder(std::uint8_t size_of_int) {
	if (size_of_int > 63) {
		this->limit = std::numeric_limits<std::uint64_t>::max();
	}
	else {
		this->limit = std::pow(2, size_of_int) - 1;
	}
}

std::uint64_t Rounder::Round_Up(std::uint64_t what_to_round_up, std::uint64_t divisor) {
	std::uint64_t result = Perform_Rounding_Up(what_to_round_up, divisor);
	if (Limit_Ok(result) == false) {
		throw std::overflow_error("Result of rounding is larger than max value of int.");
	}
	return result;
}

std::uint64_t Rounder::Perform_Rounding_Up(std::uint64_t what_to_round_up, std::uint64_t divisor) {
	if (what_to_round_up % divisor == 0) {
		//already divisible
		return what_to_round_up;
	}
	else {
		std::uint64_t share = what_to_round_up / divisor;
		return (share + 1) * divisor;
	}
}

bool Rounder::Limit_Ok(std::uint64_t result) {
	if (result > this->limit) {
		return false;
	}
	else {
		return true;
	}
}