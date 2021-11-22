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
	int Get_Path_Size() const;
public:
	CWD(char* path);

	void Append(char* path);
	int Print(char* buffer, int buffer_size) const;
};

void test_cwd();