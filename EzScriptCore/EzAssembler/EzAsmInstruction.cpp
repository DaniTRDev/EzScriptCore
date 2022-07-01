#include "../pch.h"

namespace Ez
{ 
	EzAsmInstruction::EzAsmInstruction(rage::RageInstr Instr, std::size_t OperandCount) :
		m_InstructionId(Instr), m_OperandCount(OperandCount), m_Operands(nullptr), m_IsJump(false), m_IsCall(false),
		m_IsNativeCall(false), m_IsVarPush(false), m_IsVarLoad(false), m_IsVarStore(false),
		m_IsStatic(false), m_IsLocal(false), m_IsGlobal(false),
		m_IsSwitch(false), m_IsOperandSigned(false), m_IsFloat(false), m_NativeIndex(0),
		m_NativeParameterCount(0), m_NativeReturnCount(0),
		m_JumpOffset(0)
	{}

	EzAsmInstruction::EzAsmInstruction(rage::RageInstr Instr) : EzAsmInstruction(Instr, 0)
	{
	}

	EzAsmInstruction::~EzAsmInstruction()
	{
		delete m_Operands;
	}

	rage::RageInstr EzAsmInstruction::GetInstructionId()
	{
		return m_InstructionId;
	}

	void EzAsmInstruction::SaveOperand(char* Source)
	{
		if (m_Operands)
			delete m_Operands;

		m_Operands = new char[m_OperandCount];
		std::copy_n(Source, m_OperandCount, m_Operands);
	}

	std::size_t EzAsmInstruction::GetOperandCount()
	{
		return m_OperandCount;
	}

	float EzAsmInstruction::GetOperandsF()
	{
		return *(float*)m_Operands;
	}

	std::int32_t EzAsmInstruction::GetOperandsI32()
	{
		return *(std::int32_t*)m_Operands;
	}
	std::uint32_t EzAsmInstruction::GetOperandsU32()
	{
		return *(std::uint32_t*)m_Operands;
	}

	std::uint16_t EzAsmInstruction::GetNativeIndex()
	{
		return m_NativeIndex;
	}
	std::uint8_t EzAsmInstruction::GetNativeParamCount()
	{
		return m_NativeParameterCount;
	}
	std::uint8_t EzAsmInstruction::GetNativeReturnCount()
	{
		return m_NativeReturnCount;
	}

	std::uintptr_t EzAsmInstruction::GetJumpOffset()
	{
		return m_JumpOffset;
	}
	std::string_view EzAsmInstruction::GetFuncCall()
	{
		return m_FuncCall;
	}

	bool EzAsmInstruction::IsSigned()
	{
		return m_IsOperandSigned;
	}
	bool EzAsmInstruction::IsFloat()
	{
		return m_IsFloat;
	}

	bool EzAsmInstruction::IsJump()
	{
		return m_IsJump;
	}
	bool EzAsmInstruction::IsCall()
	{
		return m_IsCall;
	}
	const std::map<std::int32_t, std::uintptr_t>& EzAsmInstruction::GetSwitchCases()
	{
		return m_Cases;
	}

	bool EzAsmInstruction::IsNativeCall()
	{
		return m_IsNativeCall;
	}
	bool EzAsmInstruction::IsVarPush()
	{
		return m_IsVarPush;
	}
	bool EzAsmInstruction::IsVarLoad()
	{
		return m_IsVarLoad;
	}
	bool EzAsmInstruction::IsVarStore()
	{
		return m_IsVarStore;
	}
	bool EzAsmInstruction::IsStatic()
	{
		return m_IsStatic;
	}
	bool EzAsmInstruction::IsLocal()
	{
		return m_IsLocal;
	}
	bool EzAsmInstruction::IsGlobal()
	{
		return m_IsGlobal;
	}
	bool EzAsmInstruction::IsSwitch()
	{
		return m_IsSwitch;
	}
}
