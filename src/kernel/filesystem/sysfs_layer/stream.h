#pragma once

#include "../vfs2/vfs_element.h"


namespace stream {

	template<class Impl>
	struct __Stream_Base : public VFS_Element2 {

		std::uint64_t Write(size_t how_many_bytes, void* buffer) {
			return static_cast<Impl*>(this)->Write(how_many_bytes, buffer);
		}

		std::uint64_t Read(size_t how_many_bytes, void* buffer) {
			return static_cast<Impl*>(this)->Read(how_many_bytes, buffer);
		}
	};

	struct Input_Console_Stream : public __Stream_Base<Input_Console_Stream> {
		std::uint64_t Read(size_t n, void* buffer);
	};

	struct Output_Console_Stream : public __Stream_Base<Input_Console_Stream> {
		std::uint64_t Write(size_t n, void* buffer);
	};

	std::tuple<std::shared_ptr<VFS_Element2>, std::shared_ptr<VFS_Element2>> Factory();
}