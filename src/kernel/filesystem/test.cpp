#include "disk.h"
#include "filesystem_info.h"
#include "fat_io.h"
#include "cluster_io.h"
#include "global_structs.h"
#include "fat_file.h"

#include "vfs_directory.h"
#include "vfs_file.h"

#include <vector>
#include <iostream>


int test1() {
	std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(1024 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(1024 * 1024, 1, 512, 12);
	std::shared_ptr<Fat_File_Factory> file_factory = std::make_shared<Fat_File_Factory>(disk, info);

	auto file1 = file_factory->Create_New_File();


	std::uint8_t* data = new std::uint8_t[10000];
	for (int i = 0; i < 10000; i++) {
		data[i] = i * 2 + 1;
	}

	file1->Change_File_Size(50);
	file1->Write_To_File(200, 10000, static_cast<void*>(data));


	std::uint16_t file_start = file1->Get_File_Start();
	std::uint64_t file_size = file1->Get_File_Size();
	auto file2 = file_factory->Get_Existing_File(file_start, file_size);

	std::uint8_t* data2 = new std::uint8_t[10000];
	file2->Read_From_File(200, 10000, data2);



	file2->Change_File_Size(30);

	file2->Remove_File();


	return 0;
}

int test2() {
	std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(1024 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(1024 * 1024, 1, 512, 12);
	std::shared_ptr<VFS_Element_Factory> element_factory = std::make_shared<VFS_Element_Factory>(disk, info);
	std::shared_ptr<VFS_Element> root = element_factory->Create_Root_Directory();

	std::uint8_t dir_attributes = 0x11;
	char dir1_name[12] = "dir1";
	char dir2_name[12] = "dir2";
	std::uint8_t file_attributes = 0x00;
	char file1_name[12] = "file1";
	std::shared_ptr<VFS_Element> directory1 = element_factory->Create_New(root, dir1_name, dir_attributes);
	std::shared_ptr<VFS_Element> directory2 = element_factory->Create_New(directory1, dir2_name, dir_attributes);
	std::shared_ptr<VFS_Element> file1 = element_factory->Create_New(directory2, file1_name, file_attributes);

	char data[25] = "Ahoj, jak se mas? :-)";
	file1->Write(5000, 25, static_cast<void*>(data));

	std::shared_ptr<VFS_Element> file2 = element_factory->Get_Existing(directory2, file1_name);

	char* data2 = new char[25];
	file2->Read(5000, 25, data2);

	std::cout << data << " = " << data2 << std::endl;
	

	TDir_Entry* entry = static_cast<TDir_Entry*>(calloc(1, sizeof(TDir_Entry)));
	directory1->Read(0, 24, static_cast<void*>(entry));

	return 1;
}

int filesystem_test() {


	/*std::shared_ptr<IDisk> disk = std::make_shared<Dummy_Disk>(256 * 1024, 512);
	std::shared_ptr<Filesystem_Info> info = std::make_shared<Filesystem_Info>(256 * 1024, 2, 512, 12);
	std::unique_ptr<Fat_IO> fat_io = std::make_unique<Fat_IO>(info, disk);

	for (int i = 0; i < 100; i++) {
		fat_io->Set_Entry_Value(i, i * 2);
	}

	fat_io->Save_Changes();


	for (int i = 0; i < 100; i++) {
		std::uint64_t val = fat_io->Get_Entry_Value(i);
		std::cout << val << std::endl;
	}*/

	test2();
	return 0;
	
}

