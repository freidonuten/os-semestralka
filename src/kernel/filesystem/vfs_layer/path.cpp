#include "path.h"

Path::Path(char* path) { 
	Make_Filenames(path);
}

void Path::Make_Filenames(char* path) {
	std::shared_ptr<Filename> current = std::make_shared<Filename>();
	int path_index = 0;
	int filename_index = 0;

	while (true) {
		switch (path[path_index]) {
		case 0:
			if (current->Get_Length() > 1) { //1 is empty because of null termination
				this->filenames.push_back(current);
			}
			return;
		case '/':
			this->filenames.push_back(current);
			current = std::make_shared<Filename>();
			filename_index = 0;
			break;
		default:
			if (filename_index >= MAX_FILENAME_SIZE) {
				//TODO Handler ERROR
				break;
			}
			current->Add_Char(path[path_index]);
			filename_index++;
			break;
		}
		path_index++;
	}
}

Path::Path() {

}

bool Path::Is_Empty() {
	return this->filenames.size() == 0;
}

int Path::Get_Path_Length() {
	int result = 0;
	for (auto filename : this->filenames) {
		result += filename->Get_Length();
	}
	result++; //NULL TERMINATION
	return result;
}

std::uint64_t Path::Read_Path(char* buffer, std::uint64_t buffer_size) {
	int count = this->filenames.size();
	return Read_Path(buffer, count, buffer_size);
}

std::uint64_t Path::Read_Path(char* buffer, int count, std::uint64_t buffer_size) {
	int path_length = Get_Path_Length();
	if (buffer_size < path_length) {
		return 0;
	}

	std::uint64_t buffer_index = 0;
	for (int i = 0; i < count; i++) {
		buffer_index += Append_Buffer(buffer, buffer_index, this->filenames[i]);
	}
	buffer[buffer_index] = 0; //NULL TERMINATION
	buffer_index++;

	return buffer_index;
	return 0;
}

void Path::Read_Filename(char* buffer) {
	int buffer_index = 0;
	if (!this->filenames.empty()) {
		buffer_index += Append_Buffer(buffer, buffer_index, this->filenames.back());
	}
	for (int i = buffer_index; i < MAX_FILENAME_SIZE; i++) {
		buffer[i] = 0;
	}
}


std::shared_ptr<Path> Path::Append(std::shared_ptr<Path> path_to_add) {
	std::shared_ptr<Path> result = std::make_shared<Path>();

	for (auto filename : this->filenames) {
		result->filenames.push_back(filename->Copy());
	}

	for (auto filename : path_to_add->filenames) {
		result->filenames.push_back(filename->Copy());
	}

	return result;
}

std::shared_ptr<Path> Path::Get_Parent() {
	int count = this->filenames.size();
	if (count > 0) {
		std::shared_ptr<Path> result = std::make_shared<Path>();

		for (int i = 0; i < count - 1; i++) {
			result->filenames.push_back(this->filenames[i]->Copy());
		}

		return result;
	}
	else {
		//TODO ERROR
	}
}

std::shared_ptr<VFS_Element> Path::Get_Element(std::shared_ptr<VFS_Element> from) {
	std::shared_ptr<VFS_Element> result = from;
	
	for (auto filename : this->filenames) {
		char current_filename[MAX_FILENAME_SIZE];
		filename->Copy_To_Array(current_filename, MAX_FILENAME_SIZE);
		result = result->Open_Child(current_filename);
	}

	return result;
}



int Path::Append_Buffer(char* buffer, int buffer_index, std::shared_ptr<Filename> filename) {
	int length = filename->Get_Length();
	filename->Copy_To_Array(buffer + buffer_index, length);
	return length;
}

void Filename::Add_Char(char character) {
	this->implementation.push_back(character);
}

void Filename::Copy_To_Array(char* result, int count) {
	int size = this->implementation.size();

	for (int i = 0; i < size && i < count; i++) {
		result[i] = this->implementation[i];
	}

	for (int i = size; i < count; i++) {
		result[i] = 0;
	}
}

int Filename::Get_Length() {
	return this->implementation.size();
}

std::shared_ptr<Filename> Filename::Copy() {
	std::shared_ptr<Filename> result = std::make_shared<Filename>();

	for (int i = 0; i < this->implementation.size(); i++) {
		result->Add_Char(this->implementation[i]);
	}

	return result;
}


