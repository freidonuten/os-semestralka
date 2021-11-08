#include "open_file_action.h"


std::shared_ptr<VFS_Element> Open_File_Action::Create(std::shared_ptr<VFS_Element> parent, char filename[MAX_FILENAME_SIZE], std::uint16_t file_attributes) {
	if (parent->Contains_Child(filename)) {
		parent->Remove_Child(filename);
	}

	return parent->Create_Child(filename, file_attributes);
}

std::shared_ptr<VFS_Element> Open_File_Action::Open(std::shared_ptr<VFS_Element> parent, char filename[MAX_FILENAME_SIZE]) {
	return parent->Open_Child(filename);
}

void Open_File_Action::Run() {
	char* path_argument = reinterpret_cast<char*>(this->registers->rdx.r);
	kiv_os::NOpen_File open_file_constants = static_cast<kiv_os::NOpen_File>(this->registers->rcx.r);
	std::uint16_t file_attributes = this->registers->rdi.r;

	auto path_to_file = vfs->Get_Path(path_argument);
	auto root_directory = this->vfs->Get_Root();

	std::shared_ptr<VFS_Element> result;
	if (path_to_file->Is_Empty()) {
		result = root_directory;
	}
	else {
		auto path_to_parent = path_to_file->Get_Parent();
		char filename[MAX_FILENAME_SIZE];
		path_to_file->Read_Filename(filename);

		std::shared_ptr<VFS_Element> parent_element = path_to_parent->Get_Element(root_directory);

		if (open_file_constants == kiv_os::NOpen_File::fmOpen_Always) {
			result = Open(parent_element, filename);
		}
		else {
			result = Create(parent_element, filename, file_attributes);
		}
	}
	

	auto table = this->vfs->Get_Descriptor_Table();
	std::uint16_t descriptor = table->Create_Descriptor(result);
	this->registers->rax.x = descriptor;
}