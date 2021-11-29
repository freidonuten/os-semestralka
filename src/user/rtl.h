#pragma once

#include "../api/api.h"
#include "../api/hal.h"
#include <atomic>
#include <string>

#define KIV_OS_EXIT(code) kiv_os_rtl::Exit(code); return code;

namespace kiv_os_rtl {

	extern std::atomic<kiv_os::NOS_Error> Last_Error;
	
	void Default_Signal_Handler();

	kiv_os::NOS_Error Read_File(const kiv_os::THandle file_handle, char* const buffer, const size_t buffer_size, size_t &read);
		//zapise do souboru identifikovaneho deskriptor data z buffer o velikosti buffer_size a vrati pocet zapsanych dat ve written
		//vraci true, kdyz vse OK
		//vraci true, kdyz vse OK

	kiv_os::NOS_Error Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written);
	//zapise do souboru identifikovaneho deskriptor data z buffer o velikosti buffer_size a vrati pocet zapsanych dat ve written
	//vraci true, kdyz vse OK
	//vraci true, kdyz vse OK

	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Read_File(const kiv_os::THandle file_handle, SizedBuffer buf) {
		size_t count;
		return { count, Read_File(file_handle, buf.data(), buf.size(), count) };
	}
	
	template<typename SizedBuffer>
	std::pair<size_t, kiv_os::NOS_Error> Write_File(const kiv_os::THandle file_handle, SizedBuffer buf) {
		size_t count;
		return { count, Write_File(file_handle, buf.data(), buf.size(), count) };
	}

	/*
		File system
	*/ 
	kiv_os::NOS_Error Open_File(const std::string& filename, std::uint8_t attributes, kiv_os::NOpen_File flags, kiv_os::THandle& open);
	kiv_os::NOS_Error Seek(kiv_os::THandle handle, kiv_os::NFile_Seek operation, kiv_os::NFile_Seek from_position, size_t& position);
	kiv_os::NOS_Error Close_Handle(kiv_os::THandle handle);
	kiv_os::NOS_Error Delete_File(const std::string& filename);
	kiv_os::NOS_Error Set_Working_Dir(const std::string& path);
	kiv_os::NOS_Error Get_Working_Dir(char* buffer, const size_t filename_lenght, size_t& chars_written);
	kiv_os::NOS_Error Create_Pipe(kiv_os::THandle *handles);
	

	// Process
	kiv_os::NOS_Error Create_Process(const std::string& name, const std::string& arguments, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle& new_process);
	kiv_os::NOS_Error Create_Thread(void* name, void* data, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle& new_process);
	kiv_os::NOS_Error Wait_For(kiv_os::THandle* handles, const size_t count_handles, kiv_os::THandle &index);
	bool Read_Exit_Code(const kiv_os::THandle handle, uint16_t& readed_exit_code);
	bool Exit(uint16_t exit_process_thread_code);
	bool Shutdown();
	bool Register_Signal_Handler(kiv_os::NSignal_Id signal_id, kiv_os::TThread_Proc thread_proc);
}