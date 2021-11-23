#include "cwd.h"
#include <iostream>

void print(CWD& cwd) {
	char buffer[256];
	cwd.Print(buffer, 256);
	std::cout << buffer << std::endl;
}

void test_cwd() {
	char* path1 = "/test1/test2/slozka/";
	char* path2 = "/test3//test4";
	char* path3 = "next";

	CWD cwd1 = CWD(path1);
	print(cwd1);
	cwd1.Append(path2);
	print(cwd1);
	cwd1.Append(path3);
	print(cwd1);

	int printed = cwd1.Print(nullptr, 10);

	for (auto it = cwd1.begin(); it < cwd1.end(); it++) {
		std::cout << *it << std::endl;
	}


	CWD cwd2 = cwd1;
	print(cwd2);
	cwd2.Append("/../../..");
	print(cwd2);
	print(cwd1);
}