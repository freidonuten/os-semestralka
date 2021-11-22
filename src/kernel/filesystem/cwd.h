#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <cstddef>

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



	struct Iterator {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::string;
		using pointer = std::string*;
		using reference = std::string&;

		Iterator(pointer ptr) : m_ptr(ptr) {}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }
		Iterator& operator++() { m_ptr++; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		friend bool operator< (const Iterator& a, const Iterator& b) { return a.m_ptr < b.m_ptr; };
		friend bool operator> (const Iterator& a, const Iterator& b) { return a.m_ptr > b.m_ptr; };

	private:
		pointer m_ptr;
	};

	Iterator begin() {
		return Iterator(implementation.data());
	}
	Iterator end() {
		return Iterator(implementation.data() + this->implementation.size());
	}
};



void test_cwd();