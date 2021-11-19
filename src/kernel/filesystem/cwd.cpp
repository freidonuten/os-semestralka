#include "cwd.h"

CWD::CWD(char* path) {
	std::stringstream temp(path);
	std::string token;

	while (std::getline(temp, token, '/')) {
		if (token.compare("") == 0) {
			continue;
		}
		this->implementation.push_back(token);
	}
}

void CWD::Merge(CWD&& other) noexcept {
	//append strings from other to current's end using std::move
	this->implementation.insert(
		this->implementation.end(),
		std::make_move_iterator(other.implementation.begin()),
		std::make_move_iterator(other.implementation.end())
	);

	other.implementation.erase(
		other.implementation.begin(),
		other.implementation.end()
	);
}

void CWD::Reduce(int levels) {
	if (levels > this->implementation.size()) {
		throw std::runtime_error("Can't reduce more path elements than available.");
	}

	for (int i = 0; i < levels; i++) {
		this->implementation.pop_back();
	}
}

void CWD::Append(char* path) {
	CWD other(path);
	this->Merge(std::move(other));
}

void CWD::Print(char* buffer) {
	char* ptr = buffer;
	for (auto path_element : this->implementation) {
		*ptr = '/';
		ptr++;

		size_t length = path_element.length();
		memcpy(ptr, path_element.c_str(), length);
		ptr += length;

	}
	*ptr = 0; //NULL TERMINATION
}