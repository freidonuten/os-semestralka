#include "proc_file.h"
#include "../../kernel.h"
#include <algorithm>


proc::File::File(const Task_Manager& tm) 
	: begin{ tm.get_processes().cbegin() }
	, end  { tm.get_processes().cend()   }
	, index{ tm.get_processes().cbegin() }
{ }

std::uint64_t proc::File::Read(size_t how_many_bytes, void* buffer) {
	const auto is_running = [](const auto& i) {
		return i.get_state() != Execution_State::FREE;
	};

	// find next active PCB slot
	index = std::find_if(index, end, is_running);

	if (index == end) {
		return 0; // PCB exhausted, EOF
	}

	// Get PCB entry and increment PCB iterator
	const auto& pcb = *index++;
	auto ss = std::stringstream();

	ss  //<< pcb.get_cwd() << "\t"
		<< pcb.get_pid() << "\t"
		<< pcb.get_name() << "\n";

	const auto& result = ss.str();
	const auto  size_limit = min(how_many_bytes, result.length());

	// copy results into target buffer
	auto buffer_end = std::memcpy(buffer, result.data(), size_limit * sizeof(char));

	return size_limit;
}

std::shared_ptr<proc::File> proc::Factory() {
	return std::make_shared<File>(task_manager);
}
