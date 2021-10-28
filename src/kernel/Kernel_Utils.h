#pragma once

#include "../api/api.h"
#include "constants.h"


namespace kut {
	constexpr bool is_proc(const kiv_os::THandle handle) {
		return handle < constants::process_limit;
	}

	template<class T>
	constexpr bool is_finished(const T& object) {
		return object.get_state() == Execution_State::FINISHED;
	}
}
