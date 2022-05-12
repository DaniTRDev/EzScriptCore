#pragma once

namespace Ez
{
	class EzSigned : public EzInstruction
	{
	public:

		EzSigned(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint8_t Size);

	};
}

