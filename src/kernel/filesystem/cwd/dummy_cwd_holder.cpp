#include "dummy_cwd_holder.h"
#include "../../Task_Manager.h"
#include "../../kernel.h"


Dummy_CWD_Holder::Dummy_CWD_Holder(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory> directory) {
	Set_Value(cwd, directory);
	task_manager.inject_cwd_holder(this);
}

void Dummy_CWD_Holder::Inherit(kiv_os::THandle ppid, kiv_os::THandle cpid) {
	cwd_mapping.insert_or_assign(cpid, cwd_mapping.at(ppid));
}

void Dummy_CWD_Holder::Set_Value(std::shared_ptr<Path> cwd, std::shared_ptr<VFS_Directory> directory) {
	cwd_mapping.insert_or_assign(task_manager.get_current_process().get_pid(), CWD_pair{ cwd, directory });
}


std::tuple<std::shared_ptr<Path>, std::shared_ptr<VFS_Directory>> Dummy_CWD_Holder::Get_Value() {
	return cwd_mapping.at(task_manager.get_current_process().get_pid());
}