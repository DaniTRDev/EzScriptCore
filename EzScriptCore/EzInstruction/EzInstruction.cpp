#include "../../EzScriptCore/pch.h"

namespace Ez
{
	EzInstruction::EzInstruction(rage::RageInstr Instr, std::uint8_t* OpCodes, std::uintptr_t StartAddr, std::uint32_t OperandCount) :
		m_InstructionId(Instr), m_OpCodes(OpCodes), m_StartAddr(StartAddr), m_OperandCount(OperandCount),
		m_EndAddr(StartAddr + OperandCount)
	{
	}
	EzInstruction::~EzInstruction()
	{
		m_StartAddr = 0;
		m_EndAddr = 0;
		m_OperandCount = 0;
		m_DecompiledInstruction.clear();
	}

	std::int32_t EzInstruction::GetOperandsI32(bool IsConsole)
	{
		switch (m_OperandCount)
		{
		case 1:
			return m_OpCodes[m_StartAddr + 1]; /*since it's one byte we don't care of endianess*/
		case 2:
			auto Val = *(std::int16_t*)&m_OpCodes[m_StartAddr + 1];
			return (IsConsole) ? Endians::Reverse16(Val) : Val;
		case 3: /*24bits*/
			auto Bytes = (std::uint8_t*)(&m_OpCodes[m_StartAddr + 1]);
			return (IsConsole) ? Util::Bits2I32(Bytes[2], Bytes[1], Bytes[0], 0) :
				Util::Bits2I32(Bytes[0], Bytes[1], Bytes[2], 0);
		case 4:
			auto Val = *(std::int32_t*)&m_OpCodes[m_StartAddr + 1];
			return (IsConsole) ? Endians::Reverse32(Val) : Val;
		}
	}
}
