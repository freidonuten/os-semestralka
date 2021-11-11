#include "pipe.h"


constexpr size_t Pipe::Base::inc(const size_t a) const {
	return (a + 1) % BUFFER_SIZE;
}

int Pipe::Base::Write(std::uint64_t offset, size_t limit, void* source_vp) {
	if (partially_closed) {
		// pipe is either closed to write => don't write
		// or pipe is closed to read
		//   => it's waste of cpu to write when nobody's going to read
		return 0;
	}

	auto source = reinterpret_cast<char*>(source_vp);
	auto index = offset;

	while (index < limit) {
		std::unique_lock<std::mutex> lock(mutex);

		while (begin == inc(end)) {
			cond_writable.wait(lock);
		}

		while (index < limit && inc(end) != begin) {
			buffer[end] = source[index++];
			end = inc(end);
		}

		cond_readable.notify_all();
	}

	return index;
}

int Pipe::Base::Read(std::uint64_t, size_t limit, void* target_vp) {
	if (Is_Closed()) {
		return 0;
	}

	// wait until there's something to read
	std::unique_lock<std::mutex> lock(mutex);

	while (begin == end) {
		cond_readable.wait(lock);
	}

	// now we're reading
	auto target = reinterpret_cast<char*>(target_vp);
	auto index = 0;

	while (index < limit && begin != end) {
		target[index++] = buffer[begin];
		begin = inc(begin);
	}

	// I read something, let's notify a writer
	cond_writable.notify_all();

	return index; // return how much has been read
}

void Pipe::Base::Close_End() {
	partially_closed = true;
}

bool Pipe::Base::Is_Closed() const {
	return partially_closed && begin == end;
}

Pipe::Write_End::Write_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

int Pipe::Write_End::Write(std::uint64_t offset, size_t limit, void* buffer) {
	return pipe->Write(offset, limit, buffer);
}

Pipe::Read_End::Read_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

int Pipe::Read_End::Read(std::uint64_t offset, size_t limit, void* buffer) {
	return pipe->Read(offset, limit, buffer);
}

Pipe::End::End(std::shared_ptr<Base> pipe) : pipe(pipe)
{ }

Pipe::End::~End() {
	pipe->Close_End();
}

Pipe::RW_Pair Pipe::Factory() {
	const auto base = std::make_shared<Base>();

	return {
		std::make_shared<Write_End>(base),
		std::make_shared<Read_End>(base)
	};
}
