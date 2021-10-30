#include "Thread_Control_Block.h"
#include "Process_Control_Block.h"


Thread_Control_Block::Thread_Control_Block(
	Process_Control_Block& parent,
	const kiv_os::TThread_Proc entry,
	const kiv_hal::TRegisters& state
) : parent(parent), context(state)
{
	instance = std::thread(entry, context); // this will throw system_error on failure
	perform_state_transition(Execution_State::RUNNING);
}

Thread_Control_Block::~Thread_Control_Block() {
	if (instance.joinable()) {
		// if native thread is still running at this point, kill it
		instance.~thread();
	}
}

void Thread_Control_Block::perform_state_transition(const Execution_State new_state) {
	if (new_state == Execution_State::FINISHED) {
		// If we are moving into zombie state, signal all exit triggers
		// and empty the que (we are only signaling once...)
		while (exit_triggers.size() > 0) {
			exit_triggers.front().get()->signal();
			exit_triggers.pop_front();
		}
	}

	state = new_state;
}

kiv_os::THandle Thread_Control_Block::get_tid_of(const std::thread::id system_id) {
	return static_cast<kiv_os::THandle>( // cast because inspections are retarded
		std::hash<std::thread::id>()(system_id)
	);
}

kiv_os::THandle Thread_Control_Block::get_tid() const {
	return Thread_Control_Block::get_tid_of(instance.get_id());
}

kiv_os::THandle Thread_Control_Block::get_ppid() const {
	return parent.get_pid();
}

Execution_State Thread_Control_Block::get_state() const {
	return state;
}

bool Thread_Control_Block::is_current() const {
	return std::this_thread::get_id() == instance.get_id();
}

void Thread_Control_Block::register_signal_handle(const kiv_os::NSignal_Id signal, const kiv_os::TThread_Proc handler) {
	signal_handlers.emplace(signal, handler);
}

void Thread_Control_Block::remove_signal_handle(const kiv_os::NSignal_Id signal) {
	signal_handlers.erase(signal);
}

void Thread_Control_Block::exit(const uint16_t exit_code) {
	this->exit_code = exit_code;

	perform_state_transition(Execution_State::FINISHED);

	ExitThread(0);
}

void Thread_Control_Block::insert_exit_trigger(std::shared_ptr<Trigger> trigger) {
	exit_triggers.push_back(trigger);
}

uint16_t Thread_Control_Block::read_exit_code()
{
	if (state != Execution_State::FINISHED) {
		throw std::runtime_error("Read_exit_code called on running thread.");
	}

	state = Execution_State::FREE;
	parent.thread_remove(get_tid());

	return exit_code;
}
