#include "../../EzScriptCore/pch.h"

namespace Ez
{
	EzInstruction::EzInstruction(rage::RageInstr Instr, std::uintptr_t StartAddr, std::uint8_t OperandCount) :
		m_InstructionId(Instr), m_StartAddr(StartAddr), m_OperandCount(OperandCount),
		m_EndAddr(StartAddr + OperandCount), m_IsJump(false), m_IsCall(false),
		m_IsNativeCall(false), m_IsVarPush(false), m_IsVarLoad(false), m_IsVarStore(false),
		m_IsStatic(false), m_IsLocal(false), m_IsGlobal(false),
		m_IsSwitch(false), m_IsOperandSigned(false), m_IsFloat(false), m_NativeIndex(0),
		m_NativeParameterCount(0), m_NativeReturnCount(0),
		m_JumpOffset(0), m_CallOffset(0), m_InstructionSize(1 + OperandCount)
	{
	}
	EzInstruction::~EzInstruction()
	{
		m_InstructionId = rage::NONE;
		m_StartAddr = 0;
		m_EndAddr = 0;
		m_OperandCount = 0;
		m_DecompiledInstruction.clear();
	}

	std::uintptr_t EzInstruction::GetStartAddr()
	{
		return m_StartAddr;
	}

	std::uintptr_t EzInstruction::GetEndAddr()
	{
		return m_EndAddr;
	}

	rage::RageInstr EzInstruction::GetInstructionId()
	{
		return m_InstructionId;
	}
	std::string_view EzInstruction::Id2String()
	{
		return rage::RageInstr2Str[m_InstructionId];
	}

	std::size_t EzInstruction::GetInstructionSize()
	{
		return m_InstructionSize;
	}

	float EzInstruction::GetOperandsF(std::uint8_t* OpCodes)
	{
		float Return;
		auto OperandVal = GetOperandsU32(OpCodes);

		auto ReturnBytes = (std::uint8_t*)&Return;
		auto OperandBytes = (std::uint8_t*)&OperandVal;

		ReturnBytes[3] = OperandBytes[3];
		ReturnBytes[2] = OperandBytes[2];
		ReturnBytes[1] = OperandBytes[1];
		ReturnBytes[0] = OperandBytes[0];

		return Return;
	}

	std::int32_t EzInstruction::GetOperandsI32(std::uint8_t* OpCodes)
	{
		switch (m_OperandCount)
		{
		case 1:
			return std::int8_t(OpCodes[m_StartAddr + 1]);
		case 2:
			return *(std::int16_t*)&OpCodes[m_StartAddr + 1];
		
		case 3: /*24bits*/
		{
			auto Bytes = (std::int8_t*)(&OpCodes[m_StartAddr + 1]);
			return Util::Bits2I32(Bytes[2], Bytes[1], Bytes[0], 0);
		}

		case 4:
			return *(std::int32_t*)&OpCodes[m_StartAddr + 1];

		}
	}
	std::uint32_t EzInstruction::GetOperandsU32(std::uint8_t* OpCodes)
	{
		switch (m_OperandCount)
		{
		case 1:
			return std::uint8_t(OpCodes[m_StartAddr + 1]);
		case 2:
			return *(std::uint16_t*)&OpCodes[m_StartAddr + 1];

		case 3: /*24bits*/
		{
			auto Bytes = (std::int8_t*)(&OpCodes[m_StartAddr + 1]);
			return std::uint32_t(Util::Bits2U32(Bytes[2], Bytes[1], Bytes[0], 0));
		}

		case 4:
			return *(std::uint32_t*)&OpCodes[m_StartAddr + 1];

		}
	}

	std::uint16_t EzInstruction::GetNativeIndex()
	{
		return m_NativeIndex;
	}
	std::uint8_t EzInstruction::GetNativeParamCount()
	{
		return m_NativeParameterCount;
	}
	std::uint8_t EzInstruction::GetNativeReturnCount()
	{
		return m_NativeReturnCount;
	}

	std::uintptr_t EzInstruction::GetJumpOffset()
	{
		return m_JumpOffset;
	}
	std::uintptr_t EzInstruction::GetCallOffset()
	{
		return m_CallOffset;
	}

	bool EzInstruction::IsSigned()
	{
		return m_IsOperandSigned;
	}
	bool EzInstruction::IsFloat()
	{
		return m_IsFloat;
	}

	bool EzInstruction::IsJump()
	{
		return m_IsJump;
	}
	bool EzInstruction::IsCall()
	{
		return m_IsCall;
	}
	const std::map<std::int32_t, std::uintptr_t>& EzInstruction::GetSwitchCases()
	{
		return m_Cases;
	}
	bool EzInstruction::IsNativeCall()
	{
		return m_IsNativeCall;
	}
	bool EzInstruction::IsVarPush()
	{
		return m_IsVarPush;
	}
	bool EzInstruction::IsVarLoad()
	{
		return m_IsVarLoad;
	}
	bool EzInstruction::IsVarStore()
	{
		return m_IsVarStore;
	}
	bool EzInstruction::IsStatic()
	{
		return m_IsStatic;
	}
	bool EzInstruction::IsLocal()
	{
		return m_IsLocal;
	}
	bool EzInstruction::IsGlobal()
	{
		return m_IsGlobal;
	}
	bool EzInstruction::IsSwitch()
	{
		return m_IsSwitch;
	}


	std::uint8_t EzInstruction::GetOperandCount()
	{
		return m_OperandCount;
	}
}
