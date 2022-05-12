#pragma once

namespace Ez
{
	enum class EzDisassemblerStatus;

	class EzSwitch : public EzInstruction
	{
	public:

		EzSwitch(rage::RageInstr Instr, std::uintptr_t OpCodeId, rage::FuncSwitch* RageSwitch);

	private:

		void AddCase(std::int32_t CaseIndex, std::uintptr_t JumpOffset);
	};
}

