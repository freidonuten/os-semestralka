#pragma once

#include "../api/api.h"


enum class Execution_State : uint8_t {
	FREE = 0,
	READY,
	RUNNING,
	FINISHED,
};
