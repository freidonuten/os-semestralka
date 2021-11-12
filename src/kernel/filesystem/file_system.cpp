#include "file_system.h"
#include "sysfs_layer/pipe.h"
#include <map>


void file_system::Dispatcher::operator()(kiv_hal::TRegisters& regs) {
	using kiv_os::NOS_File_System;
	static constexpr auto dispatch_table = [](const auto op) {
		switch (op) {
			case NOS_File_System::Close_Handle:		  return close_handle;
			case NOS_File_System::Create_Pipe:		  return create_pipe;
			case NOS_File_System::Delete_File:		  return delete_file;
			case NOS_File_System::Get_File_Attribute: return get_file_attr;
			case NOS_File_System::Get_Working_Dir:	  return get_cwd;
			case NOS_File_System::Open_File:		  return open_file;
			case NOS_File_System::Read_File:		  return read_file;
			case NOS_File_System::Seek:				  return seek;
			case NOS_File_System::Set_File_Attribute: return set_file_attr;
			case NOS_File_System::Set_Working_Dir:    return set_cwd;
			case NOS_File_System::Write_File:		  return write_file;
			default: // handle error
				throw "Handle this!";
		}
	};
	const auto call = [this, &regs](const auto& func) {
		return func(regs, vfs);
	};

	const auto service = dispatch_table(static_cast<NOS_File_System>(regs.rax.l));

	call(service);
}

void file_system::close_handle(kiv_hal::TRegisters& regs, VFS& vfs) {
	vfs.Get_Descriptor_Table()->Remove_Descriptor(regs.rdx.x);
}

void file_system::create_pipe(kiv_hal::TRegisters& regs, VFS& vfs) {
	auto [write_end, read_end] = Pipe::Factory();

	auto desc_table = vfs.Get_Descriptor_Table();
	auto ptr = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);

	// output is write end and read end of the pipe
	ptr[0] = desc_table->Create_Descriptor(write_end);
	ptr[1] = desc_table->Create_Descriptor(read_end);
}

void file_system::delete_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto path_argument = reinterpret_cast<char*>(regs.rdx.r);

	const auto root_directory = vfs.Get_Root();
	const auto path_to_file = vfs.Get_Path(path_argument);
	const auto path_to_parent = path_to_file->Get_Parent();
	const auto parent_element = path_to_parent->Get_Element(root_directory);

	char filename[MAX_FILENAME_SIZE];

	path_to_file->Read_Filename(filename);
	parent_element->Remove_Child(filename);
}

void file_system::open_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto result = [regs, &vfs]() {
		const auto path_argument = reinterpret_cast<char*>(regs.rdx.r);
		const auto open_file_constants = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
		const auto file_attributes = regs.rdi.r;

		auto path_to_file = vfs.Get_Path(path_argument);
		auto root_directory = vfs.Get_Root();

		if (path_to_file->Is_Empty()) {
			return root_directory;
		}

		auto path_to_parent = path_to_file->Get_Parent();

		char filename[MAX_FILENAME_SIZE];
		
		path_to_file->Read_Filename(filename);
		auto parent_element = path_to_parent->Get_Element(root_directory);

		if (open_file_constants == kiv_os::NOpen_File::fmOpen_Always) {
			return parent_element->Open_Child(filename);
		}

		if (parent_element->Contains_Child(filename)) {
			parent_element->Remove_Child(filename);
		}

		return parent_element->Create_Child(filename, file_attributes);
	}();

	regs.rax.x = vfs.Get_Descriptor_Table()->Create_Descriptor(result);
}

void file_system::write_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto entry = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);

	const auto written_bytes = entry->element->Write(entry->position, count, buffer);
	entry->position += written_bytes;

	regs.rax.r = written_bytes;
}

void file_system::read_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto entry = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);
	const auto read_bytes = entry->element->Read(entry->position, count, buffer);

	entry->position += read_bytes;
	regs.rax.r = read_bytes;
}

void file_system::seek(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto seek_offset = regs.rdi.r;
	const auto seek_type = static_cast<kiv_os::NFile_Seek>(regs.rcx.l);
	const auto seek_operation = static_cast<kiv_os::NFile_Seek>(regs.rcx.h);

	auto entry = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);

	switch (seek_type) {
	case kiv_os::NFile_Seek::Beginning:
		entry->position = seek_offset;
		break;
	case kiv_os::NFile_Seek::Current:
		entry->position += seek_offset;
		break;
	case kiv_os::NFile_Seek::End:
		auto dir_entry = entry->element->Generate_Dir_Entry();
		entry->position = dir_entry.file_size += seek_offset;
		break;
	}

	switch (seek_operation) {
	case kiv_os::NFile_Seek::Get_Position:
		regs.rax.r = entry->position;
		break;
	case kiv_os::NFile_Seek::Set_Size:
		entry->element->Change_Size(entry->position);
		break;
	case kiv_os::NFile_Seek::Set_Position:
		//DO NOTHING
		break;
	}
}

void file_system::get_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto path_attr = reinterpret_cast<char*>(regs.rdx.r);
	const auto root_directory = vfs.Get_Root();

	const auto path_to_file = vfs.Get_Path(path_attr);
	const auto element = path_to_file->Get_Element(root_directory);

	regs.rdi.r = element->Read_Attributes();
}

void file_system::get_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto buffer = reinterpret_cast<char*>(regs.rdx.r);
	const auto buffer_size = regs.rcx.r;

	regs.rdi.r = vfs.Get_Current_Path()->Read_Path(buffer, buffer_size);
}

void file_system::set_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto path_argument = reinterpret_cast<char*>(regs.rdx.r);
	const auto file_attributes = regs.rdi.r; // FIXME tady byl narrowing cast 64->16, takhle je to správně?
	const auto root_directory = vfs.Get_Root();

	auto path_to_file = vfs.Get_Path(path_argument);
	auto element = path_to_file->Get_Element(root_directory);

	element->Change_Attributes(file_attributes);
}

void file_system::set_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	char* path_argument = reinterpret_cast<char*>(regs.rdx.r);

	auto root_directory = vfs.Get_Root(); // FIXME co se s tímhle dělá, má to side efekty? Smazat?
	std::shared_ptr<Path> new_path = vfs.Get_Path(path_argument);
	vfs.Set_Current_Path(new_path);
}
