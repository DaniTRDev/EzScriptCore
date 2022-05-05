#include "../../pch.h"

namespace Ez
{
	EzJmp::EzJmp(rage::RageInstr Instr, std::uintptr_t StartAddr, rage::Jump* RageJump) :
		EzInstruction(Instr, StartAddr, std::uint32_t(sizeof(rage::Jump) - 1))
	{
		m_IsJump = true;
		m_JumpOffset = StartAddr + RageJump->m_JumpOffset + sizeof(rage::Jump);
		/*relative to buffer*/
	}

}
