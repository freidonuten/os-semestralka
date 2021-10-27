#pragma once

#include <mutex>
#include <condition_variable>


class Trigger {
private:
	int8_t counter = 0;

	std::mutex mutex;
	std::condition_variable condition;

public:
	void signal();
	void wait();
};


void Trigger::signal() {
	std::unique_lock<std::mutex> lock(mutex);

	++counter;

	condition.notify_all();
}

void Trigger::wait() {
	std::unique_lock<std::mutex> lock(mutex);

	while (counter <= 0) {
		condition.wait(lock);
	}
}
