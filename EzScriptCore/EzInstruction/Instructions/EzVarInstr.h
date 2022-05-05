#pragma once

namespace Ez
{
	class EzVarInstr : public EzInstruction
	{
	public:

		EzVarInstr(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint8_t Size, bool Signed);
		

	};
}
