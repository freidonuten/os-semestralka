#pragma once

#include <Windows.h>
#include <thread>
#include <unordered_map>
#include <deque>
#include "../api/api.h"
#include "State.h"
#include "Trigger.h"


class Process_Control_Block;
class Thread_Control_Block final {
private:
	const HANDLE native_handle;
	union {
		DWORD native_id;
		kiv_os::THandle tid;
	};
	Process_Control_Block &parent;
	kiv_os::TThread_Proc signal_handler;
	char** args; // null terminated strings?
	

public:
	static kiv_os::THandle current_tid();

	kiv_os::THandle get_tid() const;
	kiv_os::THandle get_ppid() const;
	Execution_State get_state() const;
	HANDLE get_native_handle() const;
	bool is_current() const;

	explicit Thread_Control_Block() = delete;
	explicit Thread_Control_Block(Process_Control_Block& parent, const kiv_os::TThread_Proc entry, const kiv_hal::TRegisters& state);

	void register_signal_handle(const kiv_os::TThread_Proc handler = nullptr);
	void signal(const kiv_os::NSignal_Id signal);
	void exit(const uint16_t exit_code);

	uint16_t read_exit_code();
};
