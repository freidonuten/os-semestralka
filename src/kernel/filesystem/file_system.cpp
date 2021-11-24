#include "file_system.h"
#include "sysfs_layer/pipe.h"
#include "utils/char_utils.h"
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
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);

	auto [cwd, dir] = vfs.Get_CWD();
	auto [dir_entry, found] = dir->Read_Entry_By_Name(filename);

	if (!found) {
		//TODO error not found
		return;
	}

	auto element = vfs.Make_File(dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);

	
	bool deleted = element->Remove();
	if (!deleted) {
		//TODO error directory not empty, unknown error for file
		return;
	}
	
	bool removed_from_dir = dir->Remove_Entry(filename);
	if (!removed_from_dir) {
		//TODO unknown ERROR
		return;
	}
}

std::tuple<std::shared_ptr<VFS_Element2>, bool> open_file_open(VFS& vfs, std::shared_ptr<VFS_Directory2> dir, char* filename) {
	auto [dir_entry, found] = dir->Read_Entry_By_Name(filename);


	if (!found) {
		//TODO ERROR filename not found
		return { nullptr, false };
	}

	auto element = vfs.Make_File(dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);

	return { element, true };
}

std::tuple<std::shared_ptr<VFS_Element2>, bool> open_file_create(VFS& vfs, std::shared_ptr<VFS_Directory2> dir, char* filename, uint64_t file_attrs) {
	dir->Remove_Entry(filename); //we will not check return value, it doesn't matter if it exists or not

	auto element = vfs.Make_File(dir->Get_Fat_Directory(), filename, file_attrs);
	element->Create();
	auto dir_entry = element->Generate_Dir_Entry();
	auto exists = dir->Create_New_Entry(dir_entry);

	if (exists) {
		//strange error, we have removed the file on line 1 this function
		bool removed = element->Remove();
		if (removed) {
			//even stranger error, this should be file or empty folder
			//now we are in inconsistent state
			return { nullptr, false };
		}
		return { nullptr, false };
	}

	return { element, true };
}

void file_system::open_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	const auto open_file_constants = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
	const auto file_attributes = regs.rdi.r;

	if (utils::Is_Valid_Filename(filename) == false) {
		//TODO ERROR invalid filename
		return;
	}

	auto [cwd, dir] = vfs.Get_CWD();

	bool is_ok;
	std::shared_ptr<VFS_Element2> element;
	if (open_file_constants == kiv_os::NOpen_File::fmOpen_Always) {
		std::tie(element, is_ok) = open_file_open(vfs, dir, filename);
	}
	else {
		std::tie(element, is_ok) = open_file_create(vfs, dir, filename, file_attributes);
	}

	if (!is_ok) {
		//TODO ERROR filename already exists for OPEN, unknown ERROR for CREATE
		return;
	}

	regs.rax.x = vfs.Get_Descriptor_Table()->Create_Descriptor(element);
}

void file_system::write_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto element = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);
	const auto written_bytes = element->Write(count, buffer);

	regs.rax.r = written_bytes;
}

void file_system::read_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto element = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);
	const auto read_bytes = element->Read(count, buffer);

	regs.rax.r = read_bytes;
}

void file_system::seek(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto seek_offset = regs.rdi.r;
	const auto seek_start = static_cast<kiv_os::NFile_Seek>(regs.rcx.l);
	const auto seek_operation = static_cast<kiv_os::NFile_Seek>(regs.rcx.h);

	auto element = vfs.Get_Descriptor_Table()->Get_Descriptor(descriptor);
	auto [position, result] = element->Seek(seek_offset, seek_start, seek_operation);

	switch(result) {
		case Seek_Result::NO_ERROR_POSITION_NOT_RETURNED:
			break;
		case Seek_Result::NO_ERROR_POSITION_RETURNED:
			regs.rax.r = position;
			break;
		case Seek_Result::ERROR_INVALID_PARAMETERS:
			//TODO INVALID PARAMETERS
			break;
		case Seek_Result::ERROR_SETTING_SIZE:
			//TODO IO ERROR
			break;
	}

	
}

void file_system::get_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	
	auto [cwd, dir] = vfs.Get_CWD();
	auto [dir_entry, found] = dir->Read_Entry_By_Name(filename);

	if (!found) {
		//TODO error not found
		return;
	}

	//TODO return dir_entry.file_attrs
}

void file_system::get_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto buffer = reinterpret_cast<char*>(regs.rdx.r);
	const auto buffer_size = regs.rcx.r;

	auto [cwd, directory] = vfs.Get_CWD();
	regs.rdi.r = cwd->Print(buffer, buffer_size);
}

void file_system::set_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	const auto file_attributes = regs.rdi.r; // FIXME tady byl narrowing cast 64->16, takhle je to správně?
	//dokonce bude 64->8, narrowing castu je vsude plno, nastesti je vypise prekladac
	
	auto [cwd, dir] = vfs.Get_CWD();
	auto [dir_entry, found] = dir->Read_Entry_By_Name(filename);

	if (!found) {
		//TODO error not found
		return;
	}

	auto element = vfs.Make_File(dir->Get_Fat_Directory(), dir_entry.file_name, dir_entry.file_attributes);
	element->Open(dir_entry.file_start, dir_entry.file_size);
	bool changed = element->Set_File_Attributes(file_attributes);
	if (!changed) {
		//TODO nemuzu zmenit soubor na slozku nebo naopak, nevim, jaky error se na to nejvic hodi
		return;
	}

	auto new_dir_entry = dir->Generate_Dir_Entry();
	bool found = dir->Change_Entry(filename, new_dir_entry);
	if (!found) {
		//TODO unknown error
		return;
	}

}

void file_system::set_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	char* path_argument = reinterpret_cast<char*>(regs.rdx.r);

	std::shared_ptr<CWD> new_cwd = std::make_shared<CWD>(path_argument);
	auto [new_dir, error] = vfs.Open_Directory(new_cwd);

	if (error == Open_Directory_Error::OK) {
		vfs.Set_CWD(new_cwd, new_dir);
	}
	else {
		//TODO ERROR
	}
}
