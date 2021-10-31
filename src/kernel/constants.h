#pragma once

namespace constants {
	// Velikosti tabulek procesů/threadů lze škálovat přes
	// process_limit, ostatní konstanty jsou závislé
	constexpr int process_limit = 16;
	constexpr int wait_obj_limit = process_limit * 4;
	constexpr int thread_limit = process_limit << 2;
	constexpr int thread_per_process = (thread_limit << 1) / process_limit;
	constexpr int fd_limit = process_limit << 2;
	constexpr int fd_per_process = (fd_limit << 1) / process_limit;
}
