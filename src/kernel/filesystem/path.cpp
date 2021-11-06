#include "path.h"

Path::Path(char* path) { 
	Append(path);
}

int Path::Get_Path_Length() {
	int result = 0;
	for (auto filename : this->filenames) {
		result += filename->Get_Length();
	}
	result++; //NULL TERMINATION
	return result;
}

void Path::Get_Path(char* buffer) {
	int count = this->filenames.size();
	Get_Path(buffer, count);
}

void Path::Get_Path(char* buffer, int count) {
	int buffer_index = 0;
	for (int i = 0; i < count; i++) {
		buffer_index += Append_Buffer(buffer, buffer_index, this->filenames[i]);
	}
	buffer[buffer_index] = 0; //NULL TERMINATION
}

void Path::Append(char* path) {
	std::shared_ptr<Filename> current = std::make_shared<Filename>();
	int path_index = 0;
	int filename_index = 0;

	while (true) {
		switch (path[path_index]) {
		case 0:
			this->filenames.push_back(current);
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

std::shared_ptr<VFS_Element> Path::Get_Element(std::shared_ptr<VFS_Element> from) {
	std::shared_ptr<VFS_Element> result = from;
	
	for (auto filename : this->filenames) {
		char current_filename[MAX_FILENAME_SIZE];
		filename->Copy_To_Array(current_filename, MAX_FILENAME_SIZE);
		result = result->Open_Child(current_filename);
	}

	return result;
}

std::shared_ptr<Path> Path::Get_Parent() {
	int count = this->filenames.size();
	if (count > 0) {
		char parent_path[MAX_PATH_SIZE];
		Get_Path(parent_path, count - 1);
		return std::make_shared<Path>(parent_path);
	}
	else {
		//TODO ERROR
	}
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


