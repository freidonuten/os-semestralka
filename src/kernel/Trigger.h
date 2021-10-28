#pragma once

#include <mutex>
#include <condition_variable>


class Trigger {
private:
	bool triggered = false;

	std::mutex mutex;
	std::condition_variable condition;

public:
	void signal();
	void wait();
};


inline void Trigger::signal() {
	if (triggered) {
		// listeners were notified already, just
		// return without suffering mutex acquisition overhead...
		return;
	}

	std::unique_lock<std::mutex> lock(mutex);

	// make this Trigger triggered
	triggered = true;

	// notify listeners about the state change
	condition.notify_all();
}

inline void Trigger::wait() {
	std::unique_lock<std::mutex> lock(mutex);

	// wait until Trigger gets triggered
	condition.wait(lock, [this]() { return triggered; });
}
