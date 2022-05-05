#include "../../pch.h"

namespace Ez
{
	EzFloatPush::EzFloatPush(rage::RageInstr Instr, std::uintptr_t OpCodeId) : 
		EzInstruction(Instr, OpCodeId, 4)
	{
		m_IsFloat = true;
	}
}