#pragma once

namespace Ez
{
	class EzFunction;

	class EzInstruction
	{
	public:

		friend class EzFunction;

		EzInstruction(rage::RageInstr Instr, std::uint8_t* OpCodes, std::uintptr_t StartAddr, std::uint32_t OperandCount);
		~EzInstruction();

		std::int32_t GetOperandsI32(bool IsConsole);

	protected:

		rage::RageInstr m_InstructionId;
		std::uint8_t* m_OpCodes;

		std::uintptr_t m_StartAddr;
		std::uintptr_t m_EndAddr;

		std::uint32_t m_OperandCount;

		std::string m_DecompiledInstruction;
	};
}
