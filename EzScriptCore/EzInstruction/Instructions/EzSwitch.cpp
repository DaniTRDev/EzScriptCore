#include "../../pch.h"

namespace Ez
{
	EzSwitch::EzSwitch(rage::RageInstr Instr, std::uintptr_t OpCodeId, rage::FuncSwitch* RageSwitch) :
		EzInstruction(Instr, OpCodeId, 1 + RageSwitch->m_NumCases * sizeof(rage::FuncSwitchCase))
	{
		m_IsSwitch = true;

		for (auto i = 0; i < RageSwitch->m_NumCases; i++)
		{
			auto Case = RageSwitch->m_Cases[i];

			/*these offsets here are ghetto as fuck, but we can't put them inside rage::FuncSwitch because
			they'll fuck the entire struct since we need contiguous memory*/

			auto CaseVal = Case.m_CaseVal; /*relative to function*/
			auto RealJumpOffset = OpCodeId + 8 + 
				i * sizeof(rage::FuncSwitchCase) + Case.m_JumpOffset; /*relative to buffer*/

			if (auto Result = AddCase(Case.m_CaseVal, RealJumpOffset);
				Result != EzDecompilerStatus::NoError)

				throw std::runtime_error(std::to_string(static_cast<std::uint8_t>(Result)));
		}

	}

	EzDecompilerStatus EzSwitch::AddCase(std::int32_t CaseIndex, std::uintptr_t JumpOffset)
	{
		/*8 = bytes for the switch param*/
		if (auto It = m_Cases.find(CaseIndex); It != m_Cases.end())
			return EzDecompilerStatus::DuplicatedSwitchCase;

		m_Cases.insert({ CaseIndex, JumpOffset });

		return EzDecompilerStatus::NoError;
	}
}