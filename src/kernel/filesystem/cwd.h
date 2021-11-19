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
public:
	CWD(char* path);

	void Append(char* path);
	void Print(char* buffer);
};

void test_cwd();