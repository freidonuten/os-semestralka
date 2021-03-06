#include "cwd.h"

Path::Path(const char* path) {
	this->Setup_Raw_Elements(path);
	this->Cleanup();
}

void Path::Setup_Raw_Elements(const char* path) {
	std::stringstream temp(path);
	std::string token;

	while (std::getline(temp, token, '\\')) {
		if (!token.size()) { //is empty
			continue;
		}

		this->implementation.push_back(token);
	}
}

void Path::Cleanup() {
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

	this->implementation.clear();

	this->implementation = new_implementation;
}

void Path::Merge(Path&& other) noexcept {
	//append strings from other to current's end using std::move
	this->implementation.insert(
		this->implementation.end(),
		std::make_move_iterator(other.implementation.begin()),
		std::make_move_iterator(other.implementation.end())
	);

	other.implementation.clear();
}

void Path::Append(const char* path) {
	Path other("");
	other.Setup_Raw_Elements(path);
	this->Merge(std::move(other));
	this->Cleanup(); //perform the cleanup only in merged directory
}

size_t Path::Get_Path_Size() const {
	size_t result = 1; //first '/'

	bool first = true;
	for (auto element : this->implementation) {
		if (!first) {
			result++; // '/' in path
		}
		
		result += element.size();
		first = false;
	}

	return result + 1; //null termination
}

size_t Path::Print(char* buffer, size_t buffer_size) const{
	size_t required_size = Get_Path_Size();

	if (buffer_size < required_size) {
		return 0;
	}

	char* ptr = buffer;
	*ptr = '\\';
	ptr++;

	bool first = true;
	for (auto path_element : this->implementation) {
		if (!first) {
			*ptr = '\\';
			ptr++;
		}
		first = false;
		
		size_t length = path_element.length();
		memcpy(ptr, path_element.c_str(), length);
		ptr += length;

	}
	*ptr = 0; //NULL TERMINATION

	return required_size;
}

std::string Path::To_String() const {
	std::string result = "\\";

	bool first = true;
	for (auto path_element : this->implementation) {
		if (!first) {
			result += "\\";
		}
		result += path_element;
		first = false;
	}

	return result;
}

void Path::Get_Filename(char* buffer) const {
	std::uint64_t size = this->implementation.size();
	if (size > 0) {
		std::string path_element = this->implementation[size - 1];
		size_t length = path_element.length();
		memcpy(buffer, path_element.c_str(), length + 1);
		return;
	}
	else {
		buffer[0] = 0;
	}
}

