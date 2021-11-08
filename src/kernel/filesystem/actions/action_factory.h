#include "action.h"

#include "close_handle_action.h"
#include "create_pipe_action.h"
#include "delete_file_action.h"
#include "get_file_attributes_action.h"
#include "get_working_dir_action.h"
#include "open_file_action.h"
#include "read_file_action.h"
#include "seek_action.h"
#include "set_file_attributes_action.h"
#include "set_working_dir_action.h"
#include "write_file_action.h"


class Action_Factory {
private:
	VFS* vfs;
public:
	Action_Factory(VFS* vfs);
	std::shared_ptr<Action> Get_Action(kiv_hal::TRegisters &regs);
};