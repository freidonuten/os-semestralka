#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

class CWD {
private:
	std::vector<std::string> implementation;
	void Merge(CWD&& other) noexcept;
public:
	CWD(char* path);

	void Reduce(int levels);
	void Append(char* path);
	void Print(char* buffer);
};

void test_cwd();