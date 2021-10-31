#pragma once

#include "..\api\api.h"
#include "../api/hal.h"
#include <atomic>
#include <string>

namespace kiv_os_rtl {

	extern std::atomic<kiv_os::NOS_Error> Last_Error;
	
	void Default_Signal_Handler();

	bool Read_File(const kiv_os::THandle file_handle, char* const buffer, const size_t buffer_size, size_t &read);
		//zapise do souboru identifikovaneho deskriptor data z buffer o velikosti buffer_size a vrati pocet zapsanych dat ve written
		//vraci true, kdyz vse OK
		//vraci true, kdyz vse OK

	bool Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written);
	//zapise do souboru identifikovaneho deskriptor data z buffer o velikosti buffer_size a vrati pocet zapsanych dat ve written
	//vraci true, kdyz vse OK
	//vraci true, kdyz vse OK

	/*
		File system
	*/ 
	bool Open_File(const std::string& filename, kiv_os::NFile_Attributes attributes, kiv_os::NOpen_File flags, kiv_os::THandle& open);
	bool Seek(kiv_os::THandle handle, kiv_os::NFile_Seek operation, kiv_os::NFile_Seek new_position, size_t& position);
	bool Close_Handle(kiv_os::THandle handle);
	bool Delete_File(const std::string& filename);
	bool Set_Working_Dir(const std::string& path);
	bool Get_Working_Dir(const std::string& filename, const size_t filename_lenght, size_t& chars_written);
	bool Create_Pipe(kiv_os::THandle *handles);
	

	// Process
	bool Create_Process(const std::string& name, const std::string& arguments, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle& new_process);
	bool Create_Thread(void* name, void* data, kiv_os::THandle handle_stdin, kiv_os::THandle handle_stdout, kiv_os::THandle& new_process);
	bool Wait_For(kiv_os::THandle* handles, const size_t count_handles, kiv_os::THandle &index);
	bool Read_Exit_Code(const kiv_os::THandle handle, uint16_t& readed_exit_code);
	bool Exit(uint16_t& exit_process_thread_code);
	bool Shutdown();
	bool Register_Signal_Handler(kiv_os::NSignal_Id signal_id, kiv_os::TThread_Proc thread_proc);
}