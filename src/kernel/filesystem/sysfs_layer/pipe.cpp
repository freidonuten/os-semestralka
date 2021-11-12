#include "pipe.h"


constexpr size_t Pipe::Base::inc(const size_t a) const {
	return (a + 1) % BUFFER_SIZE;
}

int Pipe::Base::Write(size_t limit, void* source_vp) {
	if (partially_closed) {
		// pipe is either closed to write => don't write
		// or pipe is closed to read
		//   => it's waste of cpu to write when nobody's going to read
		return 0;
	}

	auto source = reinterpret_cast<char*>(source_vp);
	auto index = 0;

	while (index < limit) {
		std::unique_lock<std::mutex> lock(mutex);
		cond_writable.wait(lock, [this]() {return filled < BUFFER_SIZE;  });

		while (index < limit && filled < BUFFER_SIZE) {
			buffer[end] = source[index++];
			end = inc(end);
			filled++;
		}

		lock.unlock();
		cond_readable.notify_all();
	}

	return index;
}

int Pipe::Base::Read(size_t limit, void* target_vp) {
	 // if write end is closed we don't need to synchronize
	std::unique_lock<std::mutex> lock(mutex);
	cond_readable.wait(lock, [this]() { return filled > 0 || partially_closed; });

	// now we're reading
	const auto target_begin = reinterpret_cast<char*>(target_vp);
	const auto target_end = target_begin + limit;
	auto target = target_begin;
	//auto index = 0;

	while (target < target_end && filled) {
		*target++ = buffer[begin];
		begin = inc(begin);
		filled--;
	}

	// I read something, let's notify a writer
	lock.unlock();
	cond_writable.notify_all();

	return target - target_begin; // return how much has been read
}

void Pipe::Base::Close_End() {
	std::lock_guard<std::mutex> mx(mutex);
	partially_closed = true;
	cond_readable.notify_all();
}

Pipe::Write_End::Write_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

int Pipe::Write_End::Write(std::uint64_t offset, size_t limit, void* buffer) {
	return pipe->Write(limit, buffer);
}

Pipe::Read_End::Read_End(std::shared_ptr<Base> pipe) : End(pipe)
{ }

int Pipe::Read_End::Read(std::uint64_t offset, size_t limit, void* buffer) {
	return pipe->Read(limit, buffer);
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

	return {
		std::make_shared<Write_End>(base),
		std::make_shared<Read_End>(base)
	};
}
