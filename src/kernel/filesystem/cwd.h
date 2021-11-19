#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

class CWD {
private:
	std::vector<std::string> implementation;
	void Merge(CWD&& other) noexcept;
	void Setup_Raw_Elements(char* path);
	void Cleanup();
	const int Get_Path_Size();
public:
	CWD(char* path);

	void Append(char* path);
	const int Print(char* buffer, int buffer_size);
};

void test_cwd();