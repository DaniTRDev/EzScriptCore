#include "../../pch.h"

namespace Ez
{
	EzSigned::EzSigned(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint8_t Size) :
		EzInstruction(Instr, OpCodeId, Size)
	{
		m_IsOperandSigned = true;
	}
}