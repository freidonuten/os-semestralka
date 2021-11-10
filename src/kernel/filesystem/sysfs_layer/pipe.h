#include "../vfs_layer/vfs_element.h"
#include <array>
#include <condition_variable>
#include <mutex>

class Pipe final : public VFS_Element {
private:
	static constexpr size_t BUFFER_SIZE = 64;

	std::array<char, BUFFER_SIZE> buffer;
	size_t begin = 0;
	size_t end = 0;

	std::mutex mutex;
	std::condition_variable cond_readable;
	std::condition_variable cond_writable;

	constexpr size_t inc(const size_t a) const;

public:
	Pipe() = default;
	Pipe(const Pipe&) = delete;
	Pipe(Pipe&&) = delete; // this should be theoretically movable but fuck it

	Pipe& operator=(const Pipe&) = delete;
	Pipe& operator=(const Pipe&&) = delete; // let's not move pipes around for now

	virtual int Write(std::uint64_t /*unused*/, size_t limit, void* buffer) override;
	virtual int Read (std::uint64_t /*unused*/, size_t limit, void* buffer) override;
};