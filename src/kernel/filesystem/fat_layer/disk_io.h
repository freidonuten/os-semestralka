#pragma once

#include "disk.h"
#include "filesystem_info.h"
#include "../utils/global_structs.h"

#include <memory>
#include <vector>


class Concrete_Disk_IO {
protected:
	std::shared_ptr<IDisk> disk;
	std::shared_ptr<Filesystem_Info> info;
public:
	Concrete_Disk_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);
	virtual void Proceed_Task(std::shared_ptr<IO_Task> task) = 0;
};

class Direct_Disk_IO : public Concrete_Disk_IO {
public:
	using Concrete_Disk_IO::Concrete_Disk_IO;
	virtual void Proceed_Task(std::shared_ptr<IO_Task> task);
};

class Via_Buffer_Disk_IO : public Concrete_Disk_IO {
public:
	using Concrete_Disk_IO::Concrete_Disk_IO;
	virtual void Proceed_Task(std::shared_ptr<IO_Task> task);
};

class Disk_IO {
private:
	std::shared_ptr<IDisk> disk;
	std::shared_ptr<Filesystem_Info> info;

	bool Are_Old_Sectors_Needed(std::vector<std::uint64_t> sectors, std::uint64_t start, size_t how_much);


public:
	Disk_IO(std::shared_ptr<IDisk> disk, std::shared_ptr<Filesystem_Info> info);

	void Proceed_Task(std::shared_ptr<IO_Task> task);
};