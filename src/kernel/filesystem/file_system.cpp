#include "file_system.h"
#include "sysfs_layer/pipe.h"
#include "utils/char_utils.h"
#include "actions/actions.h"

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
	const std::uint16_t id = regs.rdx.x;
	auto [element, result] = vfs.Get_Path_Handlers()->Close_Handle(id);

	if (result == Handle_Close_Result::CLOSED) {
		element->Close();
		//OK
	}
	else {
		//TODO ERROR not found
	}
}

void file_system::create_pipe(kiv_hal::TRegisters& regs, VFS& vfs) {
	auto [write_end, read_end] = Pipe::Factory();

	auto desc_table = vfs.Get_Handler_Table();
	auto ptr = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);

	// output is write end and read end of the pipe
	ptr[0] = desc_table->Create_Descriptor(write_end);
	ptr[1] = desc_table->Create_Descriptor(read_end);
}

void file_system::delete_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);

	Delete_Result result = actions::delete_file(vfs, filename);
	switch (result) {
	case Delete_Result::OK:
		return;
		//TODO process errors
	}
}

void file_system::open_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	const auto open_file_constants = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
	const auto file_attributes = regs.rdi.r;
	const auto [handler_id, result] = (open_file_constants == kiv_os::NOpen_File::fmOpen_Always)
		? actions::open_file(vfs, filename)
		: actions::create_file(vfs, filename, file_attributes);

	switch (result) {
	case Open_Result::OK:
		regs.rax.x = handler_id;
		return;
		//TODO process errors
	}
}



void file_system::write_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		//TODO INVALID HANDLER
		return;
	}
	const auto written_bytes = element->Write(count, buffer);

	regs.rax.r = written_bytes;
}

void file_system::read_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	const auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		//TODO INVALID HANDLER
		return;
	}
	const auto read_bytes = element->Read(count, buffer);

	regs.rax.r = read_bytes;
}

void file_system::seek(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto seek_offset = regs.rdi.r;
	const auto seek_start = static_cast<kiv_os::NFile_Seek>(regs.rcx.l);
	const auto seek_operation = static_cast<kiv_os::NFile_Seek>(regs.rcx.h);

	auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		//TODO INVALID HANDLER
		return;
	}

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

	regs.rdi.r = dir_entry.file_attributes;
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
	bool entry_changed = dir->Change_Entry(filename, new_dir_entry);
	if (!entry_changed) {
		//TODO unknown error
		return;
	}

}

void file_system::set_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	char* path = reinterpret_cast<char*>(regs.rdx.r);
	
	auto result = actions::set_cwd(vfs, path);
	switch (result) {
	case Set_CWD_Result::OK:
		//OK
		return;
	}

	//TODO ERRORS
}
