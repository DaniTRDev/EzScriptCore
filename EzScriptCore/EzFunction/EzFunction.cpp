#include "../pch.h"

namespace Ez
{
	EzFunction::EzFunction(std::uint8_t* OpCodes, std::uintptr_t StartAddres, 
		std::uintptr_t EndAddress, rage::FuncPrologue* Prologue, rage::FuncEpilogue* Epilogue) : 
		m_OpCodes(std::move(OpCodes)), m_StartAddr(StartAddres), m_EndAddr(EndAddress), 
		m_Prologue(Prologue), m_Epilogue(Epilogue), m_DecompiledInstructions(0)
	{
	}
	EzFunction::~EzFunction()
	{
		m_Instructions.clear();
		m_StartAddr = 0;
		m_EndAddr = 0;
		m_Prologue = nullptr;
		m_Epilogue = nullptr;
	}
	EzDecompilerStatus EzFunction::PreDecompile(bool Console)
	{
		m_DecompiledInstructions = 0;
		/*for predecompilation we don't need func epilogue*/
		auto StartOff = m_StartAddr + m_OpCodes[m_StartAddr + 4] + 5;
		for (auto OpCodeId = StartOff; OpCodeId < m_EndAddr - 3; OpCodeId++)
		{
			auto OpCode = m_OpCodes[OpCodeId];
			std::int32_t OperandCount = -1;
			/*-1 -> tells our predecompiler we are going to handle the opcode on our own*/

			switch (OpCode)
			{
			case rage::PUSH_CONST_U8:
				OperandCount = 1;
				break;

			case rage::PUSH_CONST_U8_U8:
				OperandCount = 2;
				break;

			case rage::PUSH_CONST_U8_U8_U8:
				OperandCount = 3;
				break;

			case rage::PUSH_CONST_U32:
			case rage::PUSH_CONST_F:
				OperandCount = 4;
				break;

			case rage::DUP:
				GetDup(static_cast<rage::RageInstr>(OpCode), OpCodeId);
				OperandCount = -1;
				break;

			case rage::NATIVE:
				OperandCount = 3;
				break;

			case rage::ENTER: /*there can't be enter opcodes, we skipped them */
				return EzDecompilerStatus::UnexpectedOpCode;

			case rage::LEAVE:
				OperandCount = sizeof(rage::FuncEpilogue) - 1; /*exclude the opcode*/
				break;

			case rage::ARRAY_U8:
			case rage::ARRAY_U8_LOAD:
			case rage::ARRAY_U8_STORE:
			case rage::LOCAL_U8:
			case rage::LOCAL_U8_LOAD:
			case rage::LOCAL_U8_STORE:
			case rage::STATIC_U8:
			case rage::STATIC_U8_LOAD:
			case rage::STATIC_U8_STORE:
			case rage::IADD_U8:
			case rage::IMUL_U8:
			case rage::IOFFSET_U8:
			case rage::IOFFSET_U8_LOAD:
			case rage::IOFFSET_U8_STORE:
				OperandCount = 1;
				break;

			case rage::PUSH_CONST_S16:
			case rage::IADD_S16:
			case rage::IMUL_S16:
			case rage::IOFFSET_S16:
			case rage::IOFFSET_S16_LOAD:
			case rage::IOFFSET_S16_STORE:
			case rage::ARRAY_U16:
			case rage::ARRAY_U16_LOAD:
			case rage::ARRAY_U16_STORE:
			case rage::LOCAL_U16:
			case rage::LOCAL_U16_LOAD:
			case rage::LOCAL_U16_STORE:
			case rage::STATIC_U16:
			case rage::STATIC_U16_LOAD:
			case rage::STATIC_U16_STORE:
			case rage::GLOBAL_U16:
			case rage::GLOBAL_U16_LOAD:
			case rage::GLOBAL_U16_STORE:
				OperandCount = 2;
				break;

			case rage::J:
			case rage::JZ:
			case rage::IEQ_JZ:
			case rage::INE_JZ:
			case rage::IGT_JZ:
			case rage::IGE_JZ:
			case rage::ILT_JZ:
			case rage::ILE_JZ:
				GetJump(static_cast<rage::RageInstr>(OpCode), OpCodeId, m_EndAddr, Console);
				OperandCount = -1;
				break;

			case rage::CALL:
			case rage::GLOBAL_U24:
			case rage::GLOBAL_U24_LOAD:
			case rage::GLOBAL_U24_STORE:
			case rage::PUSH_CONST_U24:
				OperandCount = 3;
				break;

			case rage::SWITCH: /*[OpCodes + 1] = switch cases*/
				GetSwitch(static_cast<rage::RageInstr>(OpCode), OpCodeId, Console);
				OperandCount = -1;
				break;

			case rage::TEXT_LABEL_ASSIGN_STRING:
			case rage::TEXT_LABEL_ASSIGN_INT:
			case rage::TEXT_LABEL_APPEND_STRING:
			case rage::TEXT_LABEL_APPEND_INT:
				OperandCount = 1;
				break;

			default:
				if (OpCode > 126)
					return EzDecompilerStatus::UnexpectedOpCode;

				OperandCount = 0; /*functions without operands*/
				break;
			}

			if (OperandCount != -1)
			{
				m_Instructions.push_back(std::make_unique<EzInstruction>
					(static_cast<rage::RageInstr>(OpCode), OpCodeId, OperandCount));

				OpCodeId += OperandCount;
				m_DecompiledInstructions++;
			}
		}


		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzFunction::Decompile(bool Console)
	{
		m_DecompiledInstructions = 0;

		for (auto i = 0; i < m_Instructions.size(); i++)
		{
			auto& Instr = m_Instructions[i];

			if (Instr->m_InstructionId == rage::NOP) /*skip nops*/
				continue;

			bool HasOperands = 0;
			auto OperandVal = 0;

			if (Instr->m_OperandCount > 0)
			{
				HasOperands = true;
				OperandVal = Instr->GetOperandsI32(Console);
			}

			
		}

		return EzDecompilerStatus::NoError;
	}

	std::size_t EzFunction::GetDecompiledInstrNum()
	{
		return m_DecompiledInstructions;
	}

	EzDecompilerStatus EzFunction::GetJump(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uintptr_t FuncEnd, bool Console)
	{
		auto Jump = (rage::Jump*)&m_OpCodes[OpCodeId];
		auto JumpOffset = Jump->m_JumpOffset; /*relative to function*/

		if (Console)
			JumpOffset = Endians::Reverse16(JumpOffset);

		/*relative to m_OpCodes*/
		auto RealJumpOffset = JumpOffset + std::int32_t(OpCodeId) + std::int32_t(sizeof(rage::Jump)); 

		if (RealJumpOffset < FuncEnd)
			m_Instructions.push_back(std::make_unique<EzJmp>(Instr, OpCodeId, RealJumpOffset));

		/*if the offset is outside our func we don't want to follow it, so nop it*/

		m_Instructions.push_back(std::make_unique<EzInstruction>(rage::NOP, OpCodeId, 0));
		m_Instructions.push_back(std::make_unique<EzInstruction>(rage::NOP, OpCodeId + 1, 0));
		m_Instructions.push_back(std::make_unique<EzInstruction>(rage::NOP, OpCodeId + 2, 0));

		OpCodeId += (sizeof(rage::Jump) - 1); /*we need to exclude the opcode that's why -1*/
		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzFunction::GetDup(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		auto JumpOff = 1;
		
		for (;; JumpOff++) /*we are skipping conditional instructions here, they'll need to be decompiled directly*/
		{
			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::NOP)
				continue;
			
			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::INOT)
				continue;

			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::JZ)
			{
				OpCodeId += JumpOff + 2;
				break;
			}
			m_Instructions.push_back(std::make_unique<EzInstruction>(Instr, OpCodeId, 0));
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzFunction::GetSwitch(rage::RageInstr Instr, std::uintptr_t& OpCodeId, bool Console)
	{
		auto RageSwitch = (rage::FuncSwitch*)&m_OpCodes[OpCodeId];
		auto Switch = std::make_unique<EzSwitch>(Instr, OpCodeId, 
			1 + RageSwitch->m_NumCases * sizeof(rage::FuncSwitchCase));

		for (auto i = 0; i < RageSwitch->m_NumCases; i++)
		{
			auto Case = RageSwitch->m_Cases[i];

			/*these offsets here are ghetto as fuck, but we can't put them inside rage::FuncSwitch because 
			they'll fuck the entire struct since we need contiguous memory*/

			auto CaseVal = Case.m_CaseVal; 
			auto RealJumpOffset = std::int32_t(OpCodeId) + 8 + i * 6 + Case.m_JumpOffset;

			if (Console)
			{
				RealJumpOffset = std::int32_t(OpCodeId) + 8 + i * 6 + Endians::Reverse32(Case.m_JumpOffset);
				CaseVal = Endians::Reverse32(CaseVal);
			}

			if (auto Result = Switch->AddCase(Case.m_CaseVal, RealJumpOffset);
					Result != EzDecompilerStatus::NoError)
				return Result;
		}

		m_Instructions.push_back(std::move(Switch));
		OpCodeId += (1 + sizeof(rage::FuncSwitchCase) * RageSwitch->m_NumCases);

		return EzDecompilerStatus::NoError;
	}
	
	const std::string& EzFunction::GetAssembly()
	{
		return m_ASsd.str();
	}
}