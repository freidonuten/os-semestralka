#pragma once

#include "../vfs_layer/vfs_element.h"


namespace stream {

	template<class Impl>
	struct __Stream_Base : public VFS_Element {

		int Write(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
			return static_cast<Impl*>(this)->Write(starting_byte, how_many_bytes, buffer);
		}

		int Read(std::uint64_t starting_byte, size_t how_many_bytes, void* buffer) {
			return static_cast<Impl*>(this)->Read(starting_byte, how_many_bytes, buffer);
		}
	};

	struct Input_Console_Stream : public __Stream_Base<Input_Console_Stream> {
		int Read(std::uint64_t /*unused*/, size_t n, void* buffer);
	};

	struct Output_Console_Stream : public __Stream_Base<Input_Console_Stream> {
		int Write(std::uint64_t /*unused*/, size_t n, void* buffer);
	};

	std::tuple<std::shared_ptr<VFS_Element>, std::shared_ptr<VFS_Element>> Factory();
}