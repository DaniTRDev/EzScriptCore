#include "../pch.h"

namespace Ez
{
	EzFunction::EzFunction(std::uint8_t* OpCodes, std::uintptr_t StartAddres, 
		std::uintptr_t EndAddress, std::uintptr_t Index, rage::FuncPrologue* Prologue, 
		rage::FuncEpilogue* Epilogue) :
		m_OpCodes(OpCodes), m_StartAddr(StartAddres), m_EndAddr(EndAddress), m_Index(Index),
		m_Prologue(Prologue), m_Epilogue(Epilogue), m_MappedInstructions(0)
	{
	}
	EzFunction::~EzFunction()
	{
		m_Instructions.clear();
		m_StartAddr = 0;
		m_EndAddr = 0;
		m_Prologue = nullptr;
		m_Epilogue = nullptr;
		m_MappedInstructions = 0;
	}

	void EzFunction::MapInstructions()
	{
		/*for predecompilation we don't need func epilogue*/

		auto StartOff = m_StartAddr + m_Prologue->m_FuncNameLenght + sizeof(rage::FuncPrologue);
		auto ASd = m_OpCodes[m_StartAddr];
		for (auto OpCodeId = StartOff; OpCodeId < m_EndAddr - sizeof(rage::FuncEpilogue); OpCodeId++)
		{
			auto RawOp = m_OpCodes[OpCodeId];
			auto Offset = OpCodeId - m_StartAddr;
			auto Offset2 = m_EndAddr - m_StartAddr;
			auto OpCode = rage::RageInstr(m_OpCodes[OpCodeId]);
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
				OperandCount = 4;
				break;

			case rage::PUSH_CONST_F:
				GetFloatPush(OpCode, OpCodeId);
				OperandCount = -1;
				break;

			case rage::DUP:
				GetDup(OpCode, OpCodeId);
				OperandCount = -1;
				break;

			case rage::NATIVE:
				GetNativeCall(OpCode, OpCodeId);
				OperandCount = -1;
				break;

			case rage::ENTER: /*there can't be enter opcodes, we skipped them */
				throw EzException().SetExceptionClass(typeid(this).name())
					.SetExceptionFunc(__func__)
					.SetExceptionName("Unexpected opcode")
					.SetExceptionInfo("Didn't expect an ENTER (45) opcode when mapping function' instructions");

			case rage::LEAVE:
				OperandCount = sizeof(rage::FuncEpilogue) - 1; /*exclude the opcode*/
				break;

			case rage::ARRAY_U8:
			case rage::ARRAY_U8_LOAD:
			case rage::ARRAY_U8_STORE:
				OperandCount = 1;
				break;

			case rage::LOCAL_U8:
			case rage::LOCAL_U8_LOAD:
			case rage::LOCAL_U8_STORE:
			case rage::STATIC_U8:
			case rage::STATIC_U8_LOAD:
			case rage::STATIC_U8_STORE:
				GetVarInstr(OpCode, OpCodeId, 1, false);
				OperandCount = -1;
				break;

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
				GetSigned(OpCode, OpCodeId, 2);
				OperandCount = -1;
				break;

			case rage::ARRAY_U16:
			case rage::ARRAY_U16_LOAD:
			case rage::ARRAY_U16_STORE:
				OperandCount = 2;
				break;

			case rage::LOCAL_U16:
			case rage::LOCAL_U16_LOAD:
			case rage::LOCAL_U16_STORE:
			case rage::STATIC_U16:
			case rage::STATIC_U16_LOAD:
			case rage::STATIC_U16_STORE:
			case rage::GLOBAL_U16:
			case rage::GLOBAL_U16_LOAD:
			case rage::GLOBAL_U16_STORE:
				GetVarInstr(OpCode, OpCodeId, 2, false);
				OperandCount = -1;
				break;

			case rage::J:
			case rage::JZ:
			case rage::IEQ_JZ:
			case rage::INE_JZ:
			case rage::IGT_JZ:
			case rage::IGE_JZ:
			case rage::ILT_JZ:
			case rage::ILE_JZ:
				GetJump(OpCode, OpCodeId, m_EndAddr);
				OperandCount = -1;
				break;

			case rage::CALL:
				GetCall(OpCode, OpCodeId);
				OperandCount = -1;
				break;

			case rage::GLOBAL_U24:
			case rage::GLOBAL_U24_LOAD:
			case rage::GLOBAL_U24_STORE:
				GetVarInstr(OpCode, OpCodeId, 3, false);
				OperandCount = -1;
				break;

			case rage::PUSH_CONST_U24:
				OperandCount = 3;
				break;

			case rage::SWITCH: /*[OpCodes + 1] = switch cases*/
				GetSwitch(OpCode, OpCodeId);
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
					throw EzException().SetExceptionClass(typeid(this).name())
					.SetExceptionFunc(__func__)
					.SetExceptionName("Unexpected opcode")
					.SetExceptionInfo("Didn't expect an opcode greater than 126 when mapping function' instructions");


				OperandCount = 0; /*instrs without operands*/
				break;
			}

			if (OperandCount != -1)
			{
				m_Instructions.push_back(std::make_shared<EzInstruction>
					(OpCode, OpCodeId, OperandCount));

				OpCodeId += OperandCount;
			}
		}

		m_MappedInstructions = m_Instructions.size();
	}
	std::size_t EzFunction::GetMappedInstructions()
	{
		return m_MappedInstructions;
	}
	const std::vector<EzPtr<EzInstruction>>& EzFunction::GetInstructions()
	{
		return m_Instructions;
	}
	
	void EzFunction::AddLoc(std::uintptr_t Address)
	{
		m_Locs.insert({ Address, m_Locs.size() });
	}
	const std::map<std::uintptr_t, std::uintptr_t>& EzFunction::GetLocs()
	{
		return m_Locs;
	}

	std::uintptr_t EzFunction::GetLocIdByOffset(std::uintptr_t Offset)
	{
		if (auto It = m_Locs.find(Offset); It != m_Locs.end())
			return m_Locs[Offset];

		return UINTPTR_MAX;
	}

	bool EzFunction::IsAddressALoc(std::uintptr_t Address)
	{
		for (auto& Loc : m_Locs)
		{
			if (Loc.first == Address)
				return true;
		}
		return false;
	}

	std::size_t EzFunction::GetFuncIndex()
	{
		return m_Index;
	}
	std::uintptr_t EzFunction::GetFuncStartAddr()
	{
		return m_StartAddr;
	}
	std::uintptr_t EzFunction::GetFuncEndAddr()
	{
		return m_EndAddr;
	}

	rage::FuncPrologue* EzFunction::GetPrologue()
	{
		return m_Prologue;
	}
	rage::FuncEpilogue* EzFunction::GetEpilogue()
	{
		return m_Epilogue;
	}

	void EzFunction::GetJump(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uintptr_t FuncEnd)
	{
		auto RageJump = (rage::Jump*)&m_OpCodes[OpCodeId];
		auto JumpOffset = RageJump->m_JumpOffset; /*relative to function*/

		/*relative to m_OpCodes*/
		auto RealJumpOffset = std::uintptr_t(OpCodeId) + JumpOffset + sizeof(rage::Jump);

		if (RealJumpOffset)
		{
			AddLoc(RealJumpOffset);
			m_Instructions.push_back(std::make_shared<EzJmp>(Instr, OpCodeId, RageJump));
		}
		else
		{
			m_Instructions.push_back(std::make_shared<EzInstruction>(rage::RageInstr::NOP, OpCodeId, 0));
			m_Instructions.push_back(std::make_shared<EzInstruction>(rage::RageInstr::NOP, OpCodeId + 1, 0));
			m_Instructions.push_back(std::make_shared<EzInstruction>(rage::RageInstr::NOP, OpCodeId + 2, 0));
		}

		OpCodeId += (sizeof(rage::Jump) - 1); /*we need to exclude JUMP opcode that's why -1*/
		
	}
	void EzFunction::GetDup(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		for (auto JumpOff = 1;; JumpOff++) /*we are skipping conditional instructions here, they'll need to be decompiled directly*/
		{
			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::NOP)  //also check if the following jump
				continue;												//is the one that skips the NOPs
																	   //used to align the code page
				
			
			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::INOT)
				continue;

			if (m_OpCodes[OpCodeId + JumpOff] == rage::RageInstr::JZ)
			{
				OpCodeId += JumpOff + (sizeof(rage::Jump) - 1);
				return;
			}

			break;
		}

		m_Instructions.push_back(std::make_shared<EzInstruction>(Instr, OpCodeId, 0));
	}
	void EzFunction::GetSwitch(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		auto RageSwitch = (rage::FuncSwitch*)&m_OpCodes[OpCodeId];
		auto EzSwitchInstr = std::make_shared<EzSwitch>(Instr, OpCodeId, RageSwitch);

		for (auto Case : EzSwitchInstr->GetSwitchCases())
			AddLoc(Case.second);

		m_Instructions.push_back(std::move(EzSwitchInstr));
		OpCodeId += (1 + sizeof(rage::FuncSwitchCase) * RageSwitch->m_NumCases);
	}

	void EzFunction::GetCall(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		m_Instructions.push_back(std::make_shared<EzCall>(Instr, OpCodeId, 
			(rage::Call*)&m_OpCodes[OpCodeId]));

		OpCodeId += sizeof(rage::Call) - 1;	
	}
	void EzFunction::GetNativeCall(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		m_Instructions.push_back(std::make_shared<EzNativeCall>(Instr, OpCodeId, 
			(rage::NativeCall*)&m_OpCodes[OpCodeId]));

		OpCodeId += sizeof(rage::NativeCall) - 1; /*remove the size of native call op code*/	
	}

	void EzFunction::GetVarInstr(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uint8_t Size, bool Signed)
	{
		m_Instructions.push_back(std::make_shared<EzVarInstr>(Instr, OpCodeId, Size, Signed));

		OpCodeId += Size;
	}

	void EzFunction::GetFloatPush(rage::RageInstr Instr, std::uintptr_t& OpCodeId)
	{
		m_Instructions.push_back(std::make_shared<EzFloatPush>(Instr, OpCodeId));

		OpCodeId += 4;
	}

	void EzFunction::GetSigned(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uint8_t Size)
	{
		m_Instructions.push_back(std::make_shared<EzSigned>(Instr, OpCodeId, Size));

		OpCodeId += Size;
	}
	
}