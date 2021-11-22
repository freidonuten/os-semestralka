#include "fat_directory.h"

Fat_Directory_Factory::Fat_Directory_Factory(std::shared_ptr<Fat_File_Factory> file_factory) {
	this->file_factory = file_factory;
}

std::shared_ptr<Fat_Directory> Fat_Directory_Factory::Create_New_Directory() {
	std::shared_ptr<Fat_File> file = this->file_factory->Create_New_File();
	return std::make_shared<Fat_Directory>(file);
}

std::shared_ptr<Fat_Directory> Fat_Directory_Factory::Get_Existing_Directory(std::uint16_t file_start, std::uint64_t file_size) {
	std::shared_ptr<Fat_File> file = this->file_factory->Get_Existing_File(file_start, file_size);
	return std::make_shared<Fat_Directory>(file);
}

Fat_Directory::Fat_Directory(std::shared_ptr<Fat_File> file) {
	this->file = file;
}

std::uint16_t Fat_Directory::Get_File_Start() {
	return this->file->Get_File_Start();
}

std::uint64_t Fat_Directory::Get_File_Size() {
	return this->file->Get_File_Size();
}

bool Fat_Directory::Create_New_Entry(Fat_Dir_Entry entry) {
	auto entries = Read_All_Entries();
	int index = Get_Index_Of_Searched(entries, entry.file_name);
	if (index == -1) {
		entries.push_back(entry);
		Write_Entries(entries);
		return true;
	}
	else {
		return false;
	}
	
}

std::vector<Fat_Dir_Entry> Fat_Directory::Read_All_Entries() {
	std::uint64_t file_size = this->file->Get_File_Size();
	std::uint32_t entries_count = file_size / sizeof(Fat_Dir_Entry);

	Fat_Dir_Entry* entry_array = new Fat_Dir_Entry[entries_count];

	void* buffer = static_cast<void*>(entry_array);
	this->file->Read_From_File(0, file_size, buffer);

	std::vector<Fat_Dir_Entry> entry_vector;
	entry_vector.reserve(entries_count);
	for (int i = 0; i < entries_count; i++) {
		entry_vector.push_back(entry_array[i]);
	}
	
	delete[] entry_array;

	return entry_vector;
}

std::tuple<Fat_Dir_Entry, bool> Fat_Directory::Read_Entry_By_Name(char file_name[8 + 1 + 3]) {
	std::vector<Fat_Dir_Entry> entries = Read_All_Entries();
	int index = Get_Index_Of_Searched(entries, file_name);
	if (index != -1) {
		return { entries[index], true };
	}
	else {
		Fat_Dir_Entry empty = global_structs::Create_Empty_Fat_Dir_Entry();
		return { empty, false };
	}
	
}

bool Fat_Directory::Remove_Entry(char file_name[8 + 1 + 3]) {
	std::vector<Fat_Dir_Entry> entries = Read_All_Entries();
	int index = Get_Index_Of_Searched(entries, file_name);
	if (index != -1) {
		Set_Last_Element_To_Index(entries, index);
		return true;
	}
	else {
		return false;
	}
	
}

bool Fat_Directory::Remove_Directory() {
	if (this->Is_Empty()) {
		this->file->Remove_File();
		this->file = nullptr;
		return true;
	}
	else {
		return false;
	}
}

bool Fat_Directory::Change_Entry(char old_file_name[8 + 1 + 3], Fat_Dir_Entry new_entry) {
	std::vector<Fat_Dir_Entry> entries = Read_All_Entries();
	int index = Get_Index_Of_Searched(entries, old_file_name);
	if (index != -1) {
		entries[index] = new_entry;
		Write_Entries(entries);
		return true;
	}
	else {
		return false;
	}
}

void Fat_Directory::Write_Entries(std::vector<Fat_Dir_Entry> entries) {
	std::uint64_t file_size = entries.size() * sizeof(Fat_Dir_Entry);
	void* buffer = static_cast<void*>(&entries[0]);

	file->Change_File_Size(file_size);
	file->Write_To_File(0, file_size, buffer);
}

bool Fat_Directory::Filenames_Equal(char name1[8 + 3 + 1], char name2[8 + 3 + 1]) {
	for (int i = 0; i < 8 + 3 + 1; i++) {
		if (name1[i] != name2[i]) {
			return false;
		}
	}
	return true;
}

int Fat_Directory::Get_Index_Of_Searched(std::vector<Fat_Dir_Entry> entries, char file_name[8 + 1 + 3]) {
	int count = entries.size();
	for (int i = 0; i < count; i++) {
		if (Filenames_Equal(entries[i].file_name, file_name)) {
			return i;
		}
	}
	return -1;
}

std::vector<Fat_Dir_Entry> Fat_Directory::Set_Last_Element_To_Index(std::vector<Fat_Dir_Entry> source, int index) {
	int last_element = source.size() - 1;
	source[index] = source[last_element];
	source.pop_back();
	return source;
}

bool Fat_Directory::Is_Empty() {
	return this->file->Get_File_Size() == 0;
}