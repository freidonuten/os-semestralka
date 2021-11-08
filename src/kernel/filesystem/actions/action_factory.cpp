#include "action_factory.h"

Action_Factory::Action_Factory(VFS* vfs) {
	this->vfs = vfs;
}

std::shared_ptr<Action> Action_Factory::Get_Action(kiv_hal::TRegisters &registers) {
	kiv_os::NOS_File_System operation = static_cast<kiv_os::NOS_File_System>(registers.rax.l);

	switch (operation) {

	case kiv_os::NOS_File_System::Close_Handle:
		return std::make_shared<Close_Handle_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Create_Pipe:
		return std::make_shared<Create_Pipe_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Delete_File:
		return std::make_shared<Delete_File_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Get_File_Attribute:
		return std::make_shared<Get_File_Attributes_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Get_Working_Dir:
		return std::make_shared<Get_Working_Dir_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Open_File:
		return std::make_shared<Open_File_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Read_File:
		return std::make_shared<Read_File_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Seek:
		return std::make_shared<Seek_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Set_File_Attribute:
		return std::make_shared<Set_File_Attributes_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Set_Working_Dir:
		return std::make_shared<Set_Working_Dir_Action>(registers, vfs);

	case kiv_os::NOS_File_System::Write_File:
		return std::make_shared<Write_File_Action>(registers, vfs);
	}
}