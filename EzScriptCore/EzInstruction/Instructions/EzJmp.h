#pragma once

namespace Ez
{
	class EzJmp : public EzInstruction
	{
	public:

		EzJmp(rage::RageInstr Instr, std::uintptr_t StartAddr, std::int32_t JumpOffset);

		std::int32_t GetJumpOffset();

	private:

		std::int32_t m_JumpOffset;
	};
}

