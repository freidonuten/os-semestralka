#pragma once

#include "../vfs_layer/vfs_element.h"
#include <array>
#include <condition_variable>
#include <mutex>


namespace Pipe {
	class Base {
	private:
		static constexpr size_t BUFFER_SIZE = 4096;

		std::array<char, BUFFER_SIZE> buffer = {};
		size_t begin = 0;
		size_t end = 0;
		size_t filled = 0;
		bool partially_closed = false;

		std::mutex mutex;
		std::condition_variable cond_readable;
		std::condition_variable cond_writable;

		constexpr size_t inc(const size_t a) const;

	public:
		Base() = default;
		Base(const Base&) = delete;
		Base(Base&&) = delete;

		Base& operator=(const Base&) = delete;
		Base& operator=(const Base&&) = delete;

		int Write(size_t limit, void* buffer);
		int Read (size_t limit, void* buffer);

		void Close_End();
	};

	template <class Derived> // CRTP
	class End : public VFS_Element {
	protected:
		const std::shared_ptr<Base> pipe;

		End(std::shared_ptr<Base> pipe);

	public:
		End() = delete;
		~End();

		int Write(std::uint64_t offset, size_t limit, void* buffer) override {
			return static_cast<Derived*>(this)->Write(offset, limit, buffer);
		}

		int Read(std::uint64_t offset, size_t limit, void* buffer) override {
			return static_cast<Derived*>(this)->Read(offset, limit, buffer);
		};
	};

	struct Write_End final : public End<Write_End> {
		Write_End() = delete;
		Write_End(std::shared_ptr<Base> pipe);
		
		virtual int Write(std::uint64_t offset, size_t limit, void* buffer) override;
	};

	struct Read_End final : public End<Read_End> {
		Read_End() = delete;
		Read_End(std::shared_ptr<Base> pipe);

		virtual int Read(std::uint64_t offset, size_t limit, void* buffer) override;
	};

	using RW_Pair = std::tuple<
		std::shared_ptr<Pipe::Write_End>,
		std::shared_ptr<Pipe::Read_End>
	>;

	RW_Pair Factory();

}