#include "../../pch.h"

namespace Ez
{
	EzCall::EzCall(rage::RageInstr Instr, std::uintptr_t StartAddr, rage::Call* RageCall) :
		EzInstruction(Instr, StartAddr, std::uint32_t(sizeof(rage::Call) - 1))
	{
		m_IsCall = true;
		m_CallOffset = Util::Bits2U32(RageCall->m_CallOff3, RageCall->m_CallOff2, RageCall->m_CallOff1, 0);
	}
}