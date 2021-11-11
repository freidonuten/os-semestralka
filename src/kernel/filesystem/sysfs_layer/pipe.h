#pragma once

#include "../vfs_layer/vfs_element.h"
#include <array>
#include <condition_variable>
#include <mutex>


namespace Pipe {
	class Base final : public VFS_Element {
	private:
		static constexpr size_t BUFFER_SIZE = 32;

		std::array<char, BUFFER_SIZE> buffer = {};
		size_t begin = 0;
		size_t end = 0;
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

		virtual int Write(std::uint64_t offset, size_t limit, void* buffer) override;
		virtual int Read (std::uint64_t /*unused*/, size_t limit, void* buffer) override;

		void Close_End();
		bool Is_Closed() const;
	};

	class End : public VFS_Element {
	protected:
		std::shared_ptr<Base> pipe;

		End(std::shared_ptr<Base> pipe);

	public:
		End() = delete;
		~End();
	};

	struct Write_End final : public End {
		Write_End() = delete;
		Write_End(std::shared_ptr<Base> pipe);
		
		virtual int Write(std::uint64_t offset, size_t limit, void* buffer) override;
	};

	struct Read_End final : public End {
		Read_End() = delete;
		Read_End(std::shared_ptr<Base> pipe);

		virtual int Read(std::uint64_t offset, size_t limit, void* buffer) override;
	};



}