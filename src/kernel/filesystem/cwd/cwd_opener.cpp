#include "cwd_opener.h"

#include "../utils/char_utils.h"

CWD_Opener::CWD_Opener(std::shared_ptr<VFS_Directory2> root, std::shared_ptr<VFS_Fat_Element_Factory2> factory) {
	this->root = root;
	this->factory = factory;
}

std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error> CWD_Opener::Open_Directory(std::shared_ptr<CWD> cwd) {
	auto current = this->root;
	for (auto it = cwd->begin(); it < cwd->end(); it++) {
		char temp[MAX_FILENAME_SIZE];
		Char_Utils::Copy_Array(temp, (*it).c_str(), MAX_FILENAME_SIZE);
		auto [dir_entry, found] = current->Read_Entry_By_Name(temp);

		if (!found) {
			return std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error>(nullptr, Open_Directory_Error::PATH_NOT_EXISTING);
		}

		auto element = this->factory->Make(nullptr, dir_entry.file_name, dir_entry.file_attributes);
		auto directory = std::dynamic_pointer_cast<VFS_Directory2>(element);

		if (directory == NULL) {
			if (it + 1 == cwd->end()) {
				return std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error>(nullptr, Open_Directory_Error::NOT_A_DIRECTORY);
			}
			else {
				return std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error>(nullptr, Open_Directory_Error::PATH_NOT_EXISTING);
			}
		}

		directory->Open(dir_entry.file_start, dir_entry.file_size);
		current = directory;
	}

	return std::tuple<std::shared_ptr<VFS_Directory2>, Open_Directory_Error>(current, Open_Directory_Error::OK);
}