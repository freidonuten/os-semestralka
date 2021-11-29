#include "file_system.h"
#include "sysfs_layer/pipe.h"
#include "utils/char_utils.h"
#include "actions/actions.h"

#include <map>
#include "../kernel.h"


file_system::Dispatcher::Dispatcher(uint16_t sector_size, uint64_t sector_count, int drive_id)
	: vfs(sector_size, sector_count, drive_id)
{ }

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
		Set_Error(kiv_os::NOS_Error::Success, regs);
	}
	else if(result == Handle_Close_Result::NOT_EXISTS) {
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
	}

	Set_Error(kiv_os::NOS_Error::Unknown_Error, regs);
}

file_system::Dispatcher file_system::factory() {
	kiv_hal::TRegisters regs;

	for (regs.rdx.l = 0; ; regs.rdx.l++) {
		kiv_hal::TDrive_Parameters params;		
		regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Drive_Parameters);;
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&params);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
			
		if (!regs.flags.carry) {
				uint16_t sector_size = params.bytes_per_sector;
				uint64_t sector_count = params.absolute_number_of_sectors;
				int drive_id = regs.rdx.l;

				return Dispatcher(sector_size, sector_count, drive_id);
		}

		if (regs.rdx.l == 255) {
			break;
		}
	}

	// no disk found, use virtual fs
	return Dispatcher();
}

void file_system::create_pipe(kiv_hal::TRegisters& regs, VFS& vfs) {
	auto [write_end, read_end] = Pipe::Factory();

	auto desc_table = vfs.Get_Handler_Table();
	auto ptr = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);

	// output is write end and read end of the pipe
	ptr[0] = desc_table->Create_Descriptor(write_end);
	ptr[1] = desc_table->Create_Descriptor(read_end);

	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::delete_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);

	Delete_Result result = actions::delete_file(vfs, filename);
	switch (result) {
	case Delete_Result::OK:
		Set_Error(kiv_os::NOS_Error::Success, regs);
		return;
	case Delete_Result::FILE_OPENED:
	case Delete_Result::CANT_REMOVE:
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	case Delete_Result::FILE_NOT_EXISTING:
		Set_Error(kiv_os::NOS_Error::File_Not_Found, regs);
		return;
	}
	
	Set_Error(kiv_os::NOS_Error::Unknown_Error, regs);
}

void file_system::open_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	const auto open_file_constants = static_cast<kiv_os::NOpen_File>(regs.rcx.r);

	auto[ file_attributes, is_ok] = utils::toUInt8(regs.rdi.r);
	if (!is_ok) {
		Set_Error(kiv_os::NOS_Error::Invalid_Argument, regs);
		return;
	}

	const auto [handler_id, result] = (open_file_constants == kiv_os::NOpen_File::fmOpen_Always)
		? actions::open_file(vfs, filename)
		: actions::create_file(vfs, filename, file_attributes);

	switch (result) {
	case Open_Result::OK:
		regs.rax.x = handler_id;
		Set_Error(kiv_os::NOS_Error::Success, regs);
		return;
	case Open_Result::ALREADY_OPENED:
	case Open_Result::CANT_REMOVE_PREVIOUS:
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	case Open_Result::INVALID_FILENAME:
	case Open_Result::INVALID_FILE_TYPE:
		Set_Error(kiv_os::NOS_Error::Invalid_Argument, regs);
		return;
	case Open_Result::FILE_NOT_FOUND:
		Set_Error(kiv_os::NOS_Error::File_Not_Found, regs);
		return;
	case Open_Result::NO_MEMORY:
		Set_Error(kiv_os::NOS_Error::Out_Of_Memory, regs);
		return;
	}

	Set_Error(kiv_os::NOS_Error::Unknown_Error, regs);
}



void file_system::write_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	}
	const auto written_bytes = element->Write(count, buffer);

	regs.rax.r = written_bytes;
	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::read_file(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto buffer = reinterpret_cast<char*>(regs.rdi.r);
	const auto count = regs.rcx.r;

	const auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	}
	const auto read_bytes = element->Read(count, buffer);

	regs.rax.r = read_bytes;
	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::seek(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto descriptor = regs.rdx.x;
	const auto seek_offset = regs.rdi.r;
	const auto seek_start = static_cast<kiv_os::NFile_Seek>(regs.rcx.l);
	const auto seek_operation = static_cast<kiv_os::NFile_Seek>(regs.rcx.h);

	auto element = vfs.Get_Handler_Table()->Get_Element(descriptor);
	if (!element) {
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
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
			Set_Error(kiv_os::NOS_Error::Invalid_Argument, regs);
			return;
		case Seek_Result::ERROR_SETTING_SIZE:
			Set_Error(kiv_os::NOS_Error::IO_Error, regs);
			return;
	}

	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::get_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	
	auto [cwd, dir] = vfs.Get_CWD();
	auto [dir_entry, found] = dir->Read_Entry_By_Name(filename);

	if (!found) {
		Set_Error(kiv_os::NOS_Error::File_Not_Found, regs);
		return;
	}

	regs.rdi.r = dir_entry.file_attributes;
	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::get_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	auto buffer = reinterpret_cast<char*>(regs.rdx.r);
	const auto buffer_size = regs.rcx.r;

	auto [cwd, directory] = vfs.Get_CWD();
	regs.rdi.r = cwd->Print(buffer, buffer_size);
	Set_Error(kiv_os::NOS_Error::Success, regs);
}

void file_system::set_file_attr(kiv_hal::TRegisters& regs, VFS& vfs) {
	const auto filename = reinterpret_cast<char*>(regs.rdx.r);
	auto [file_attributes, is_ok] = utils::toUInt8(regs.rdi.r);
	if (!is_ok) {
		Set_Error(kiv_os::NOS_Error::Invalid_Argument, regs);
		return;
	}
	
	auto result = actions::set_file_attrs(vfs, filename, file_attributes);
	switch (result) {
	case Set_File_Attrs_Result::OK:
		Set_Error(kiv_os::NOS_Error::Success, regs);
		return;
	case Set_File_Attrs_Result::FILE_OPENED:
	case Set_File_Attrs_Result::CANT_CHANGE:
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	case Set_File_Attrs_Result::FILE_NOT_EXISTING:
		Set_Error(kiv_os::NOS_Error::File_Not_Found, regs);
		return;
	}

	Set_Error(kiv_os::NOS_Error::Unknown_Error, regs);
}

void file_system::set_cwd(kiv_hal::TRegisters& regs, VFS& vfs) {
	char* path = reinterpret_cast<char*>(regs.rdx.r);
	
	auto result = actions::set_cwd(vfs, path);
	switch (result) {
	case Set_CWD_Result::OK:
		Set_Error(kiv_os::NOS_Error::Success, regs);
		return;
	case Set_CWD_Result::INVALID_PATH:
		Set_Error(kiv_os::NOS_Error::Invalid_Argument, regs);
		return;
	case Set_CWD_Result::NOT_A_DIRECTORY:
		Set_Error(kiv_os::NOS_Error::IO_Error, regs);
		return;
	case Set_CWD_Result::PATH_NOT_FOUND:
		Set_Error(kiv_os::NOS_Error::File_Not_Found, regs);
		return;
	}

	Set_Error(kiv_os::NOS_Error::Unknown_Error, regs);
}
