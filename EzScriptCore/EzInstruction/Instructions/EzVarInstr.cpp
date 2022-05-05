#include "../../pch.h"

namespace Ez
{
	EzVarInstr::EzVarInstr(rage::RageInstr Instr, std::uintptr_t OpCodeId, std::uint8_t Size, 
		bool Signed) : EzInstruction(Instr, OpCodeId, Size)
	{
		m_IsOperandSigned = Signed;

		switch (Instr)
		{
		case rage::STATIC_U8:
		case rage::STATIC_U16:
			m_IsStatic = true;
			m_IsVarPush = true;
			break;

		case rage::STATIC_U8_LOAD:
		case rage::STATIC_U16_LOAD:
			m_IsStatic = true;
			m_IsVarLoad = true;
			break;

		case rage::STATIC_U8_STORE:
		case rage::STATIC_U16_STORE: /*in rdr2 -> locals have 24bits opcodes as well, need to add these opcodes*/
			m_IsStatic = true;
			m_IsVarStore = true;
			break;

		case rage::LOCAL_U8:
		case rage::LOCAL_U16:
			m_IsLocal = true;
			m_IsVarPush = true;
			break;

		case rage::LOCAL_U8_LOAD:
		case rage::LOCAL_U16_LOAD:
			m_IsLocal = true;
			m_IsVarLoad = true;
			break;

		case rage::LOCAL_U8_STORE:
		case rage::LOCAL_U16_STORE: 
			m_IsLocal = true;
			m_IsVarStore = true;
			break;

		case rage::GLOBAL_U16:
		case rage::GLOBAL_U24:
			m_IsGlobal = true;
			m_IsVarPush = true;
			break;

		case rage::GLOBAL_U16_LOAD:
		case rage::GLOBAL_U24_LOAD:
			m_IsGlobal = true;
			m_IsVarLoad = true;
			break;

		case rage::GLOBAL_U16_STORE:
		case rage::GLOBAL_U24_STORE:
			m_IsGlobal = true;
			m_IsVarStore = true;
			break;
		}

	}

}
