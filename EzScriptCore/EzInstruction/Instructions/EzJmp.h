#pragma once

namespace Ez
{
	class EzJmp : public EzInstruction
	{
	public:

		EzJmp(rage::RageInstr Instr, std::uintptr_t StartAddr, rage::Jump* RageJump);

	};
}

