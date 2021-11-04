#include "actions.h"

std::shared_ptr<File_Descriptor> Actions::Create_New_File(std::shared_ptr<Fat_Directory> parent, char file_name[12], std::uint16_t attributes) {
	std::shared_ptr<Fat_File> file = this->file_factory->Create_New_File();
	Add_Entry_To_Parent(parent, file, file_name, attributes);
	return this->descriptor_table->Create_Descriptor(file->Get_File_Start());
	
}

std::shared_ptr<File_Descriptor> Actions::Open_File(std::shared_ptr<Fat_Directory> parent, char file_name[12]) {
	Fat_Dir_Entry dir_entry = parent->Read_Entry_By_Name(file_name);
	return this->descriptor_table->Create_Descriptor(dir_entry.file_start);
}

void Actions::Write_To_File(std::uint16_t file_descriptor) {
	std::shared_ptr<File_Descriptor> descriptor = this->descriptor_table->Get_Descriptor(file_descriptor);
	//this->file_factory->Get_Existing_File(descriptor->fat)
}

void Actions::Add_Entry_To_Parent(std::shared_ptr<Fat_Directory> parent, std::shared_ptr<Fat_File> file, char file_name[12], std::uint16_t attributes) {
	std::uint16_t file_start = file->Get_File_Start();
	std::uint64_t file_size = file->Get_File_Size();

	Fat_Dir_Entry entry = Fat_Dir_Entry_Factory::Create(attributes, file_name, file_start, file_size);
	parent->Create_New_Entry(entry);
}