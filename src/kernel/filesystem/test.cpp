#include "test.h"

#include "../../api/api.h"
#include "../../api/hal.h"

#include <iostream>
#include <thread>

void set_working_dir(char* path) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Set_Working_Dir);
	regs.rdx.r = reinterpret_cast<uint64_t>(path);
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
}

size_t write_file(int handle, void* buffer, int how_many_bytes) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Write_File);
	regs.rdx.x = handle;
	regs.rdi.r = reinterpret_cast<uint64_t>(buffer);
	regs.rcx.r = how_many_bytes;
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);

	return regs.rax.r;
}

size_t read_file(int handle, void *buffer, int how_many_bytes) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Read_File);
	regs.rdx.x = handle;
	regs.rdi.r = reinterpret_cast<uint64_t>(buffer);
	regs.rcx.r = how_many_bytes;
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);

	return regs.rax.r;
}

int open_file(char* path) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File);
	regs.rdx.r = reinterpret_cast<uint64_t>(path);
	regs.rcx.r = 1;
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
	return regs.rax.x;
}

int create_dir(char* path) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File);
	regs.rdx.r = reinterpret_cast<uint64_t>(path);
	regs.rcx.r = 0;
	regs.rdi.r = 0x11; //directory + readonly
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
	return regs.rax.x;
}

int create_file(char* path) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File);
	regs.rdx.r = reinterpret_cast<uint64_t>(path);
	regs.rcx.r = 0;
	regs.rdi.r = 0x0;
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
	return regs.rax.x;
}

void close_handle(int handle) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle);
	regs.rdx.x = handle;
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
}

void seek(int handle, int offset, kiv_os::NFile_Seek whence) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Seek);
	regs.rdx.x = handle;
	regs.rdi.r = offset;
	regs.rcx.l = static_cast<uint8_t>(whence);
	regs.rcx.h = static_cast<uint8_t>(kiv_os::NFile_Seek::Set_Position);
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
}

void pipe(kiv_os::THandle *handles) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(kiv_os::NOS_Service_Major::File_System);
	regs.rax.l = static_cast<uint8_t>(kiv_os::NOS_File_System::Create_Pipe);
	regs.rdx.r = reinterpret_cast<uint64_t>(handles);
	kiv_hal::Call_Interrupt_Handler(kiv_os::System_Int_Number, regs);
}

char buffer1[256] = "Slunce je zlatou skobou na vobloze přibitý, pod sluncem sedlo kožený";
char buffer2[256] = "";

void writer(kiv_os::THandle handle) {
	int wrote;
	char* begin = buffer1;
	size_t remaining = strlen(buffer1);

	while (wrote = write_file(handle, begin, remaining)) {
		begin += wrote;
		remaining -= wrote;
	}
}

void reader(kiv_os::THandle handle) {
	int read;
	char* begin = buffer2;

	while (read = read_file(handle, begin, 30)) {
		begin += read;
	}
}

void test_pipes() {
	kiv_os::THandle handles[2] = { 0 };
	pipe(handles);

	std::thread t_writer(writer, handles[0]);
	std::thread t_reader(reader, handles[1]);

	t_writer.join();
	t_reader.join();

	return;
}

void filesystem_test() {
	kiv_hal::TRegisters registers;

	for (int i = 0; i < 15; i++) {
		char dir_name[12];
		sprintf_s(dir_name, "dir%d", i);
		create_dir(dir_name);
	}

	kiv_os::TDir_Entry entries[15];
	char root_name[12] = "";
	int root_handle = open_file(root_name);
	read_file(root_handle, static_cast<void*>(entries), 15 * sizeof(kiv_os::TDir_Entry));

	for (int i = 0; i < 15; i++) {
		std::cout << entries[i].file_name << std::endl;
	}

	char path[256];
	int temp;

	sprintf_s(path, "dir0");
	temp = create_dir(path);
	close_handle(temp);

	sprintf_s(path, "dir0/dir1");
	temp = create_dir(path);
	close_handle(temp);

	sprintf_s(path, "dir0/dir1/dir2");
	temp = create_dir(path);
	set_working_dir(path);
	close_handle(temp);

	sprintf_s(path, "dir3");
	temp = create_dir(path);
	close_handle(temp);

	sprintf_s(path, "dir3/dir4");
	temp = create_dir(path);
	close_handle(temp);

	sprintf_s(path, "/");
	set_working_dir(path);

	sprintf_s(path, "dir0/dir1/dir2/dir3/dir4/dir5");
	temp = create_dir(path);
	set_working_dir(path);
	close_handle(temp);

	sprintf_s(path, "file");
	int file_handle = create_file(path);

	std::uint64_t numbers[500];
	for (int i = 0; i < 500; i++) {
		numbers[i] = i * 100;
	}

	write_file(file_handle, static_cast<void*>(numbers), 500 * sizeof(std::uint64_t));
	seek(file_handle, 250 * sizeof(std::uint64_t), kiv_os::NFile_Seek::Beginning);
	read_file(file_handle, numbers, 250 * sizeof(std::uint64_t));

	for (int i = 0; i < 250; i++) {
		std::cout << numbers[i] << " = " << numbers[i + 250] << std::endl;
	}
}
