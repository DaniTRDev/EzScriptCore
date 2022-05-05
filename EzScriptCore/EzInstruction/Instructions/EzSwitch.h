#pragma once

namespace Ez
{
	enum class EzDecompilerStatus;

	class EzSwitch : public EzInstruction
	{
	public:

		EzSwitch(rage::RageInstr Instr, std::uintptr_t OpCodeId, rage::FuncSwitch* RageSwitch);

	private:

		EzDecompilerStatus AddCase(std::int32_t CaseIndex, std::uintptr_t JumpOffset);
	};
}

