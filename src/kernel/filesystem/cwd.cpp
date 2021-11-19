#include "cwd.h"

CWD::CWD(char* path) {
	this->Setup_Raw_Elements(path);
	this->Cleanup();
}

void CWD::Setup_Raw_Elements(char* path) {
	std::stringstream temp(path);
	std::string token;

	while (std::getline(temp, token, '/')) {
		if (token.compare("") == 0) {
			continue;
		}

		this->implementation.push_back(token);
	}
}

void CWD::Cleanup() {
	//we will iterate through old implementation (which includes .. a .)
	//we will ignore .
	//for .. we will remove previous element
	//we will copy the rest to new implementation

	std::vector<std::string> new_implementation;

	for (auto element : this->implementation) {
		if (element.compare(".") == 0) {
			continue;
		}

		if (element.compare("..") == 0) {
			if (new_implementation.size() > 0) {
				new_implementation.pop_back();
			}
			continue;
		}

		new_implementation.push_back(std::move(element));
	}

	this->implementation.erase(
		this->implementation.begin(),
		this->implementation.end()
	);

	this->implementation = new_implementation;
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

void CWD::Append(char* path) {
	CWD other("");
	other.Setup_Raw_Elements(path);
	this->Merge(std::move(other));
	this->Cleanup(); //perform the cleanup only in merged directory
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