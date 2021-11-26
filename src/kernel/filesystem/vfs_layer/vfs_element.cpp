#pragma once

#include "vfs_element.h"
#include "../utils/bit_utils.h"

#include "vfs_file.h"
#include <stdexcept>

VFS_Element_Factory::VFS_Element_Factory(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info) {
	this->file_factory = std::make_shared<Fat_File_Factory>(disk, info);
	this->directory_factory = std::make_shared<Fat_Directory_Factory>(this->file_factory);
}

std::shared_ptr<VFS_Element> VFS_Element_Factory::Create(std::shared_ptr<Fat_Directory> parent_directory, char file_name[12], std::uint8_t file_attributes) {
	bool is_directory = Is_Directory(file_attributes);
	std::shared_ptr<VFS_Element> result;

	if (is_directory) {
		result = std::make_shared<VFS_Directory>(this, this->directory_factory, parent_directory, file_name, file_attributes);
	}
	else {
		result = std::make_shared<VFS_File>(this->file_factory, parent_directory, file_name, file_attributes);
	}

	return result;
}


std::shared_ptr<VFS_Element> VFS_Element_Factory::Create_Root_Directory() {
	char filename[12] = "";
	std::shared_ptr<VFS_Element> result = std::make_shared<Root_Directory>(
		this, this->directory_factory, nullptr, filename, 0xff);

	result->Create();
	return result;
}


bool VFS_Element_Factory::Is_Directory(std::uint8_t file_attributes) {
	return Bit_Utils::Check_Individual_Bit(file_attributes, 0x10);
}

void VFS_Element::Create() {
	throw std::runtime_error("Unsupported operation CREATE.");
}

void VFS_Element::Open(std::uint16_t file_start, std::uint16_t file_size) {
	throw std::runtime_error("Unsupported operation OPEN.");
}

bool VFS_Element::Remove() {
	throw std::runtime_error("Unsupported operation OPEN.");
}

int VFS_Element::Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	throw std::runtime_error("Unsupported operation WRITE.");
}

int VFS_Element::Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
	throw std::runtime_error("Unsupported operation READ.");
}

void VFS_Element::Change_Size(std::uint32_t desired_size) {
	throw std::runtime_error("Unsupported operation CHANGE_SIZE.");
}

void VFS_Element::Change_Attributes(std::uint16_t file_attributes) {
	this->file_attributes = file_attributes;
	Fat_Dir_Entry entry = this->Generate_Dir_Entry();
	this->parent_fat_directory->Change_Entry(this->file_name, entry);
}

std::uint16_t VFS_Element::Read_Attributes() {
	return this->file_attributes;
}

char* VFS_Element::Read_File_Name() {
	return this->file_name;
}

std::shared_ptr<VFS_Element> VFS_Element::Create_Child(char file_name[12], std::uint16_t file_attributes) {
	throw std::runtime_error("Unsupported operation CREATE_CHILD.");
}

std::shared_ptr<VFS_Element> VFS_Element::Open_Child(char file_name[12]) {
	throw std::runtime_error("Unsupported operation OPEN_CHILD.");
}

bool VFS_Element::Contains_Child(char file_name[12]){
	throw std::runtime_error("Unsupported operation READ.");
}

void VFS_Element::Remove_Child(char file_name[12]) {
	throw std::runtime_error("Unsupported operation REMOVE_CHILD.");
}

std::shared_ptr<Fat_Dir_Entry> VFS_Element::Get_ChildO(char file_name[12]) {
	throw std::runtime_error("Unsupported operation GET_CHILD_O.");
}

void VFS_Element::Update_ChildO(char old_file_name[12], Fat_Dir_Entry entry) {
	throw std::runtime_error("Unsupported operation UPDATE_CHILD_O.");
}

Fat_Dir_Entry VFS_Element::Generate_Dir_Entry() {
	throw std::runtime_error("Unsupported operation GENERATE_DIR_ENTRY.");
}



