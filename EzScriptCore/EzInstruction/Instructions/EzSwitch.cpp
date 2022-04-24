#include "../../pch.h"

namespace Ez
{
	EzSwitch::EzSwitch(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint32_t OperandCount) : 
		EzInstruction(Instr, OpCodeId, OperandCount)
	{
	}

	EzDecompilerStatus EzSwitch::AddCase(std::int32_t CaseIndex, std::int16_t JumpOffset)
	{
		/*8 = bytes for the switch param*/
		if (auto It = m_Cases.find(CaseIndex); It != m_Cases.end())
			return EzDecompilerStatus::DuplicatedSwitchCase;

		m_Cases.insert({ CaseIndex, JumpOffset });

		return EzDecompilerStatus::NoError;
	}
}