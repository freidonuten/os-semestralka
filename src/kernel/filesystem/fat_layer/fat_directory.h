#pragma once

#include "fat_file.h"
#include "../utils/global_structs.h"

#include <memory>
#include <vector>



class Fat_Directory {
private:
	std::shared_ptr<Fat_File> file;

	bool Write_Entries(std::vector<Fat_Dir_Entry> entries);
	bool Filenames_Equal(char name1[8 + 3 + 1], char name2[8 + 3 + 1]);
	int Get_Index_Of_Searched(std::vector<Fat_Dir_Entry> entries, char file_name[8 + 1 + 3]);
	std::vector<Fat_Dir_Entry> Set_Last_Element_To_Index(std::vector<Fat_Dir_Entry> source, int index);
	bool Is_Empty();

public:
	Fat_Directory(std::shared_ptr<Fat_File> file);

	std::uint16_t Get_File_Start();
	std::uint64_t Get_File_Size();

	std::vector<Fat_Dir_Entry> Read_All_Entries();
	Create_New_Entry_Result Create_New_Entry(Fat_Dir_Entry entry);
	std::tuple<Fat_Dir_Entry, bool> Read_Entry_By_Name(char file_name[8 + 1 + 3]); //bool out = > true = ok, false = not_found
	bool Remove_Entry(char file_name[8 + 1 + 3]); //out => true = ok, false = not_found
	bool Change_Entry(char old_file_name[8 + 1 + 3], Fat_Dir_Entry new_entry); //out => true = ok, false = not_found

	bool Remove_Directory(); //out => true = ok, false = not_empty


};



class Fat_Directory_Factory {
private:
	std::shared_ptr<Fat_File_Factory> file_factory;
public:
	Fat_Directory_Factory(std::shared_ptr<Fat_File_Factory> file_factory);

	std::tuple<std::shared_ptr<Fat_Directory>, bool> Create_New_Directory();
	std::shared_ptr<Fat_Directory> Get_Existing_Directory(std::uint16_t file_start, std::uint64_t file_size);
};