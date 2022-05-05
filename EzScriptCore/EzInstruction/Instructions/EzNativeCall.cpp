#include "../../pch.h"

namespace Ez
{
	EzNativeCall::EzNativeCall(rage::RageInstr Instr, std::uintptr_t OpCodeId, rage::NativeCall* RageNative) :
		EzInstruction(Instr, OpCodeId, sizeof(rage::NativeCall) - 1)
	{
		m_NativeIndex = Util::Bits2U16(RageNative->m_NativeIdHighBits, RageNative->m_NativeIdLowBits); /*endian swap*/
		m_NativeParameterCount = std::uint8_t(RageNative->m_ParamsAndReturns >> 2);
		m_NativeReturnCount = std::uint8_t(RageNative->m_ParamsAndReturns & 0x3);

		m_IsNativeCall = true;
	}
}
