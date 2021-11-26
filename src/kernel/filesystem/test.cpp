#include "test.h"

#include "../../api/api.h"
#include "../../api/hal.h"

#include <iostream>
#include <thread>
#include <random>

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

constexpr size_t TEST_LENGTH = 1 << 20;
constexpr size_t WRITE_BATCH = TEST_LENGTH / 1024;
char buffer1[TEST_LENGTH] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
char buffer2[TEST_LENGTH] = "";

void writer(kiv_os::THandle handle) {
	for (size_t i = 0; i < TEST_LENGTH; i += WRITE_BATCH) {
		write_file(handle, buffer1 + i, WRITE_BATCH);
	}
	close_handle(handle);
}

void reader(kiv_os::THandle handle) {
	int read;
	char* begin = buffer2;

	while (read = read_file(handle, begin, 32)) {
		begin += read;
	}
}

void test_pipes() {
	constexpr auto N = 64;
	size_t time_total = 0;

	std::random_device rd;

	for (auto i = 40; i < TEST_LENGTH; ++i) {
		buffer1[i] = rd();
	}

	for (int i = 0; i < N; ++i) {
		kiv_os::THandle handles[2] = { 0 };
		pipe(handles);

		const auto start = std::chrono::high_resolution_clock::now();

		std::thread t_writer(writer, handles[0]);
		std::thread t_reader(reader, handles[1]);

		t_writer.join();
		t_reader.join();

		const auto end = std::chrono::high_resolution_clock::now();
		const auto time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		
		time_total += time_elapsed;

		std::cout << time_elapsed << "[us]\n";
	}
	std::cout <<"cmp result: " << memcmp(buffer1, buffer2, TEST_LENGTH) << "\n";

	std::cout << "avg: " << time_total / N / 1000. << "[ms]\n";
	std::cout << "looping now, kill me...\n";
	while (1);
}

void descriptor_perftest() {
	kiv_os::THandle handles[2] = { 0 };
	size_t time_total = 0;
	constexpr auto N = 1024 << 2;

	for (int i = 0; i < N; ++i) {
		const auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < 1000; ++i) {
			pipe(handles);
			close_handle(handles[0]);
			close_handle(handles[1]);
		}

		const auto end = std::chrono::high_resolution_clock::now();
		const auto time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		time_total += time_elapsed;
	}
	std::cout << "avg: " << time_total / N << "[us]\n";
	std::cout << "looping now, kill me...\n";
	while (1);
}

void filesystem_test() {
	kiv_hal::TRegisters registers;
	char filename[12];
	sprintf_s(filename, "directory");
	auto dir_handler = create_dir(filename);
	set_working_dir(filename);
	
	for (int i = 0; i < 15; i++) {
		sprintf_s(filename, "dir%d", i);
		auto temp = create_dir(filename);
		close_handle(temp);
	}

	kiv_os::TDir_Entry entries[15];

	read_file(dir_handler, static_cast<void*>(entries), 15 * sizeof(kiv_os::TDir_Entry));

	for (int i = 0; i < 15; i++) {
		std::cout << entries[i].file_name << std::endl;
	}

	char path[256];
	int temp;

	sprintf_s(path, "dir0");
	temp = create_dir(path);
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
