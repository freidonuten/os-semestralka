#include "pipe.h"


void Pipe::Base::advance_end() {
	// rotate end pointer by one
	// this is actually faster than a = b + 1 (mod c)
	if (++end == buffer.cend()) {
		end = buffer.begin();
	}
	++filled;
}

void Pipe::Base::advance_begin() {
	// rotate begin pointer by one
	if (++begin == buffer.cend()) {
		begin = buffer.begin();
	}
	--filled;
}

std::uint64_t Pipe::Base::Write(size_t limit, void* source_vp) {
	if (partially_closed) {
		// pipe is either closed to write => don't write
		// or pipe is closed to read
		//   => it's waste of cpu to write when nobody's going to read
		return 0;
	}

	const auto source_begin = reinterpret_cast<char*>(source_vp);
	const auto source_end = source_begin + limit;
	auto source = source_begin;

	while (source < source_end) { // read until input buffer is exhausted
		std::unique_lock<std::mutex> lock(mutex);
		cond_writable.wait(lock, [this]() { return filled < BUFFER_SIZE; });

		while (source < source_end && filled < BUFFER_SIZE) {
			*end = *source++;
			advance_end();
		}

		// source buffer could not be exhausted as internal buffer is already full
		// let the readers consume what has been written and try again in the next iteration.
		cond_readable.notify_all();
	}

	return source - source_begin;
}

std::uint64_t Pipe::Base::Read(size_t limit, void* target_vp) {
	 // if write end is closed we don't need to synchronize
	std::unique_lock<std::mutex> lock(mutex);
	cond_readable.wait(lock, [this]() { return filled > 0 || partially_closed; });

	// now we're reading
	const auto target_begin = reinterpret_cast<char*>(target_vp);
	const auto target_end = target_begin + limit;
	auto target = target_begin;

	// read from internal buffer until it's empty or the target buffer is full
	while (target < target_end && filled) {
		*target++ = *begin;
		advance_begin();
	}

	// Something has been read, notify writer
	cond_writable.notify_all();

	// if pipe is exhausted, insert EOF marker
	if (partially_closed && filled == 0 && (target - target_begin) < limit) {
		*target++ = '\x04';
	}

	return target - target_begin; // return how much has been read
}

void Pipe::Base::Close_End() {
	std::lock_guard<std::mutex> mx(mutex);
	partially_closed = true;
	cond_readable.notify_all();
}

Pipe::Write_End::Write_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

std::uint64_t Pipe::Write_End::Read(size_t limit, void* buffer) {
	return VFS_Element::Read(limit, buffer);
}

std::uint64_t Pipe::Write_End::Write(size_t limit, void* buffer) {
	return pipe->Write(limit, buffer);
}

Pipe::Read_End::Read_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

std::uint64_t Pipe::Read_End::Read(size_t limit, void* buffer) {
	return pipe->Read(limit, buffer);
}

std::uint64_t Pipe::Read_End::Write(size_t limit, void* buffer) {
	return VFS_Element::Write(limit, buffer);
}

template <class Derived>
Pipe::End<Derived>::End(std::shared_ptr<Pipe::Base> pipe) : pipe(pipe)
{ }

template <class Derived>
Pipe::End<Derived>::~End() {
	pipe->Close_End();
}

Pipe::RW_Pair Pipe::Factory() {
	const auto base = std::make_shared<Base>();

	return { // base is injected to both ends, that's why I need shared pointer
		std::make_shared<Write_End>(base),
		std::make_shared<Read_End>(base)
	};
}
