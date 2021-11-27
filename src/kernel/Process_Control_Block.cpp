#include <algorithm>
#include "Process_Control_Block.h"


Process_Control_Block::Process_Control_Block(const kiv_os::THandle pid)
	: pid(pid)
{ }

Process_Control_Block::Process_Control_Block(const kiv_os::THandle pid, const kiv_os::THandle tid)
	: pid(pid), tid(tid), state(Execution_State::RUNNING)
{
	thread_list.emplace(tid, pid);
}

kiv_os::THandle Process_Control_Block::get_pid() const {
	return pid;
}

kiv_os::THandle Process_Control_Block::get_tid() const {
	return tid;
}

Execution_State Process_Control_Block::get_state() const {
	return state;
}

const char* Process_Control_Block::get_name() const {
	return name;
}

bool Process_Control_Block::is_main_thread(const kiv_os::THandle tid) const {
	return this->tid == tid;
}

kiv_os::THandle Process_Control_Block::thread_insert(
	const kiv_os::TThread_Proc entry_point, const kiv_hal::TRegisters& context
) {
	// this is a bit messy, the problem is that there's no
	// move/copy contructor for TCB (due to std::thread)
	// and there's no way to directly emplace tid/tcb pair
	// into thread list as the tid is not available before
	// TCB is contructed
	thread_list.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(0),
		std::forward_as_tuple(pid, entry_point, context)
	);

	// extract the internal node handle and change the key
	auto node = thread_list.extract(0);
	const auto tid = // store tid for return value
		node.key() = node.mapped().get_tid();

	// reinsert extracted node handle, now with different key
	thread_list.insert(std::move(node));

	// if this was the first thread, set state to running
	if (thread_list.size() == 1) {
		this->tid = tid;
	}

	return tid;
}

void Process_Control_Block::signal(const kiv_os::NSignal_Id signal) {
	thread_list.at(tid).signal(signal);
}

Thread_Control_Block& Process_Control_Block::get_thread(const kiv_os::THandle handle) {
	return thread_list.at(handle);
}

void Process_Control_Block::thread_remove(const kiv_os::THandle tid) {
	// removing main thread => process can be recycled
	if (this->tid == tid) {
		thread_list.clear();
		state = Execution_State::FREE;
		return;
	}

	// otherwise just remove the thread
	thread_list.erase(tid);
}

void Process_Control_Block::allocate() {
	state = Execution_State::READY;
}

void Process_Control_Block::exit(const uint16_t code)
{
}

void Process_Control_Block::terminate() {
	// let threads finish gracefully by signalling terminate
	std::for_each(thread_list.begin(), thread_list.end(), [](auto& thread) {
		thread.second.signal(kiv_os::NSignal_Id::Terminate);
	});

	// now bring the big hammer
	thread_remove(tid);
}
