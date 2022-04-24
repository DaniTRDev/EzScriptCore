#pragma once

namespace Ez
{
	class EzFunction;

	class EzInstruction
	{
	public:

		friend class EzFunction;

		EzInstruction(rage::RageInstr Instr, std::uintptr_t StartAddr, std::uint32_t OperandCount);
		~EzInstruction();

		std::int32_t GetOperandsI32(bool IsConsole, std::uint8_t* OpCodes);

	protected:

		rage::RageInstr m_InstructionId;

		std::uintptr_t m_StartAddr;
		std::uintptr_t m_EndAddr;

		std::uint32_t m_OperandCount;

		std::string m_DecompiledInstruction;
	};
}
