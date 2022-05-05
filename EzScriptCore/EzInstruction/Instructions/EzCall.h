#pragma once

namespace Ez
{
	class EzCall : public EzInstruction
	{
	public:

		EzCall(rage::RageInstr Instr, std::uintptr_t StartAddr, rage::Call* RageCall);

	};
}

