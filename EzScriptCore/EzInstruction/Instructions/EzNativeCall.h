#pragma once

namespace Ez
{
	class EzNativeCall : public EzInstruction
	{
	public:

		EzNativeCall(rage::RageInstr Instr, std::uintptr_t OpCodeId, rage::NativeCall* RageNative);

	};
}

