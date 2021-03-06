#pragma once

#include "../vfs_layer/vfs_element.h"

#include <array>
#include <condition_variable>
#include <mutex>


namespace Pipe {
	// FIFO pipe with support for one reader/writer
	class Base {
	private:
		static constexpr size_t BUFFER_SIZE = 4096;

		using buffer_type = std::array<char, BUFFER_SIZE>;

		buffer_type buffer = {};
		buffer_type::iterator begin = buffer.begin();
		buffer_type::iterator end = buffer.begin();
		size_t filled = 0;
		bool partially_closed = false;

		std::mutex mutex;
		std::condition_variable cond_readable;
		std::condition_variable cond_writable;

		void advance_end();
		void advance_begin();

	public:
		Base() = default;
		Base(const Base&) = delete;
		Base(Base&&) = delete;

		Base& operator=(const Base&) = delete;
		Base& operator=(const Base&&) = delete;

		std::uint64_t Write(size_t limit, void* buffer);
		std::uint64_t Read (size_t limit, void* buffer);

		void Close_End();
	};

	// Generic end of a Base pipe, implements the common VFS_Element interface
	template <class Derived> // CRTP
	class End : public VFS_Element {
	protected:
		const std::shared_ptr<Base> pipe;

		End(std::shared_ptr<Base> pipe);

	public:
		End() = delete;
		~End();

		std::uint64_t Write(size_t limit, void* buffer) override {
			return static_cast<Derived*>(this)->Write(limit, buffer);
		}

		std::uint64_t Read(size_t limit, void* buffer) override {
			return static_cast<Derived*>(this)->Read(limit, buffer);
		};
	};

	// Write end of the pipe, prohibits reading
	struct Write_End final : public End<Write_End> {
		Write_End() = delete;
		Write_End(std::shared_ptr<Base> pipe);
		
		std::uint64_t Read(size_t limit, void* buffer) override;
		std::uint64_t Write(size_t limit, void* buffer) override;
	};

	// Read end, prohibits writing to underlying pipe
	struct Read_End final : public End<Read_End> {
		Read_End() = delete;
		Read_End(std::shared_ptr<Base> pipe);

		std::uint64_t Read(size_t limit, void* buffer) override;
		std::uint64_t Write(size_t limit, void* buffer) override;
	};

	using RW_Pair = std::tuple<
		std::shared_ptr<Pipe::Write_End>,
		std::shared_ptr<Pipe::Read_End>
	>;

	// creates Write_End and Read_End backed by common Base
	RW_Pair Factory();

}