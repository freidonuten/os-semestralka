#pragma once

#include <thread>
#include <unordered_map>
#include <deque>
#include "../api/api.h"
#include "Process_Control_Block.h"
#include "State.h"
#include "Trigger.h"


class Thread_Control_Block final {
private:
	using Signal_Handler_Table = std::unordered_map<kiv_os::NSignal_Id, kiv_os::TThread_Proc>;

	Process_Control_Block* parent; // lifecycle is not managed here thus naked ptr
	uint16_t exit_code;
	kiv_hal::TRegisters context;
	std::thread instance;
	Execution_State state = Execution_State::FREE;
	Signal_Handler_Table signal_handlers;
	char** args; // null terminated strings?
	
	// Synchronization
	std::deque<std::shared_ptr<Trigger>> exit_triggers;

	void perform_state_transition(const Execution_State new_state);

public:
	static kiv_os::THandle get_tid_of(const std::thread::id system_id);

	kiv_os::THandle get_tid() const;
	kiv_os::THandle get_ppid() const;
	Execution_State get_state() const;
	bool is_current() const;

	// TODO: state manipulation
	void allocate(const kiv_os::TThread_Proc& entry, const kiv_hal::TRegisters& regs);
	void adopt(Process_Control_Block& parent);
	explicit Thread_Control_Block() = delete;
	explicit Thread_Control_Block(Process_Control_Block& parent, const kiv_os::TThread_Proc entry, const kiv_hal::TRegisters& state);

	void register_signal_handle(const kiv_os::NSignal_Id signal, const kiv_os::TThread_Proc handler);
	void remove_signal_handle(const kiv_os::NSignal_Id signal);
	void exit(const uint16_t code);
	void insert_exit_trigger(std::shared_ptr<Trigger> trigger);

	uint16_t read_exit_code();
};
