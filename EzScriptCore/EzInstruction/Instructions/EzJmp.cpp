#include "../../pch.h"

namespace Ez
{
	EzJmp::EzJmp(rage::RageInstr Instr, std::uintptr_t StartAddr, std::int32_t JumpOffset) :
		EzInstruction(Instr, StartAddr, std::uint32_t(sizeof(rage::Jump) - 1)), m_JumpOffset(JumpOffset)
	{
	}
	
	std::int32_t EzJmp::GetJumpOffset()
	{
		return m_JumpOffset;
	}
}
