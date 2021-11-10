#include "pipe.h"


constexpr size_t Pipe::inc(const size_t a) const {
	return (a + 1) % BUFFER_SIZE;
}

int Pipe::Write(std::uint64_t, size_t limit, void* source_vp) {
	std::unique_lock<std::mutex> lock(mutex);

	while (begin == inc(end)) {
		cond_writable.wait(lock);
	}

	auto source = reinterpret_cast<char*>(source_vp);
	auto index = 0;

	while (index < limit && begin != inc(end)) {
		buffer[end] = source[index++];
		end = inc(end);
	}

	cond_readable.notify_one();

	return index;
}

int Pipe::Read(std::uint64_t, size_t limit, void* target_vp) {
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
	cond_writable.notify_one();

	return index; // return how much has been read
}
