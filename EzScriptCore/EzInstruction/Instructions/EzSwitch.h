#pragma once

namespace Ez
{
	enum class EzDecompilerStatus;

	class EzSwitch : public EzInstruction
	{
	public:

		EzSwitch(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint32_t OperandCount);

		EzDecompilerStatus AddCase(std::int32_t CaseIndex, std::int16_t JumpOffset);

	private:

		std::unordered_map<std::int32_t, std::int16_t> m_Cases; /*operator[caseIndex] -> jump of*/
		std::int16_t m_DefaultJumpOffset;
	};
}

