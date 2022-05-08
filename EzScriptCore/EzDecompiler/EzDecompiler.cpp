#include "../pch.h"

namespace Ez
{
	EzDecompiler::EzDecompiler(EzSrcProgram* Script) : m_ScriptBuffer(std::move(Script->m_ScriptBuff)),
		m_OpCodeId(0), m_Script(Script), m_MappedInstructions(0), m_DisassembledInstructions(0)
	{
	}
	Ez::EzDecompiler::~EzDecompiler()
	{
		m_OpCodeId = 0;
		m_Script = nullptr;
		m_Locals.clear();
		m_ScriptParams.clear();
		m_Strings.clear();
		m_Functions.clear();
		FreeDisassemblyBuffer();
	}

	const std::vector<rage::EzLocal>& EzDecompiler::GetLocals()
	{
		return m_Locals;
	}
	const std::vector<rage::EzScriptParam>& EzDecompiler::GetScriptParams()
	{
		return m_ScriptParams;
	}
	const std::vector<std::string>& EzDecompiler::GetStrings()
	{
		return m_Strings;
	}

	const std::map<std::size_t, std::uint64_t>& EzDecompiler::GetNatives()
	{
		return m_NativeMap;
	}

	const std::size_t EzDecompiler::GetBlocksNumber()
	{
		return m_Script->m_CodeBlocks;
	}
	const std::size_t EzDecompiler::GetCodeSize()
	{
		return m_CodeBuffer->GetBufferSize();
	}
	std::uint8_t* EzDecompiler::GetCode()
	{
		return m_CodeBuffer->GetData();
	}

	const std::vector<EzFunction*>& EzDecompiler::GetFunctions()
	{
		return m_Functions;
	}

	std::string EzDecompiler::GetScriptName()
	{
		return m_Script->m_ScriptName;
	}

	EzDecompilerStatus EzDecompiler::MapScript()
	{
		if (auto Result = MapLocals(); Result != EzDecompilerStatus::NoError)
			return Result;

		if (auto Result = MapStrings(); Result != EzDecompilerStatus::NoError)
			return Result;

		if (auto Result = MapNatives(); Result != EzDecompilerStatus::NoError)
			return Result;

		if (auto Result = MapCodeBlocks(); Result != EzDecompilerStatus::NoError)
			return Result;

		if (auto Result = MapFunctions(); Result != EzDecompilerStatus::NoError)
			return Result;

		if (auto Result = MapInstructionsFromFuncs(); Result != EzDecompilerStatus::NoError)
			return Result;

		return EzDecompilerStatus::NoError;
	}

	EzDecompilerStatus EzDecompiler::MapLocals()
	{
		if (!m_Script->m_Header)
			return EzDecompilerStatus::InvalidScriptHeader;

		auto StaticsCount = m_Script->m_Header->m_StaticsCount;
		auto ParameterCount = m_Script->m_Header->m_ParameterCount;

		m_ScriptBuffer->SetPos(m_Script->m_Header->m_StaticsOffset.m_Offset + m_Script->m_RSC7Offset);

		for (auto i = 0; i < StaticsCount - ParameterCount; i++)
		{
			std::int64_t Value = 0;
			m_ScriptBuffer->Read((char*)&Value, sizeof(Value));

			m_Locals.push_back(rage::EzLocal{ std::uint32_t(m_Locals.size()), Value });
		}

		for (auto i = StaticsCount - ParameterCount; i < StaticsCount; i++)
		{
			std::int64_t Value = 0;
			m_ScriptBuffer->Read((char*)&Value, sizeof(Value));

			m_ScriptParams.push_back(rage::EzScriptParam{ std::uint32_t(m_Locals.size()), Value });
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::MapStrings()
	{
		if (!m_Script->m_Header)
			return EzDecompilerStatus::InvalidScriptHeader;

		/*first map string tables and merge them in a single table*/
		auto StringsSize = m_Script->m_Header->m_StringsSize;
		EzBuffer StringTable;
		StringTable.AllocateBuffer(StringsSize);

		for (auto i = 0; i < m_Script->m_StringBlocks; i++)
		{
			auto TableSize = ((i + 1) * 0x4000 >= StringsSize) ? StringsSize % 0x4000 : 0x4000;
			auto Table = std::make_unique<std::uint8_t[]>(TableSize);

			m_ScriptBuffer->SetPos(m_Script->m_StringTableOffsets[i]);
			m_ScriptBuffer->Read(Table, TableSize);

			StringTable.Write(std::move(Table), TableSize);
		}

		auto Data = StringTable.GetData();
		auto DataSize = StringTable.GetBufferSize();

		std::string String;

		for (auto i = 0; i < DataSize; i++)
		{
			auto Byte = Data[i];

			switch (Byte)
			{
			case rage::SI_StringAddNewLine:
				String.push_back('\n');
				break;

			case rage::SI_StringAddCarriageReturn:
				String.push_back('\r');
				break;

			case rage::SI_StringAddDoubleQuote:
				String.push_back('"');
				break;

			case rage::SI_StringFinish:
				m_Strings.push_back(String);
				String.clear();
				break;

			default:
				String.push_back((char)Byte);
				break;

			}
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::MapNatives()
	{
		m_ScriptBuffer->SetPos(m_Script->m_Header->m_NativesOffset.m_Offset + m_Script->m_RSC7Offset);
		auto CodeSize = m_Script->m_Header->m_CodeLength;

		for (auto i = 0; i < m_Script->m_Header->m_NativesCount; i++)
		{
			auto EncryptedNatVal = std::int64_t(0);
			auto Key = CodeSize + i;

			m_ScriptBuffer->Read((char*)&EncryptedNatVal, sizeof(EncryptedNatVal));
			auto Decrypted = std::rotl<std::uint64_t>(EncryptedNatVal, Key); 

			m_NativeMap.insert({ i, Decrypted });
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::MapCodeBlocks()
	{
		if (!m_Script->m_Header)
			return EzDecompilerStatus::InvalidScriptHeader;

		auto CodeLenght = m_Script->m_Header->m_CodeLength;

		m_CodeBuffer = std::make_unique<EzBuffer>();
		m_CodeBuffer->AllocateBuffer(CodeLenght);

		for (auto i = 0; i < m_Script->m_CodeBlocks; i++)
		{
			auto TableSize = (std::int32_t((i + 1) * 0x4000) >= CodeLenght) ? CodeLenght % 0x4000 : 0x4000;
			auto Block = std::make_unique<std::uint8_t[]>(TableSize);

			m_ScriptBuffer->SetPos(m_Script->m_CodeTableOffsets[i]);
			m_ScriptBuffer->Read(Block, TableSize);
			m_CodeBuffer->Write(std::move(Block), TableSize);
		}

		return EzDecompilerStatus::NoError;
	}

	EzDecompilerStatus EzDecompiler::MapFunctions()
	{
		if (!m_CodeBuffer)
			return EzDecompilerStatus::CodeBlocksNotMapped;

		auto OpCodes = m_CodeBuffer->GetData();

		bool OnFunc = false;
		std::uintptr_t FuncStart = UINTPTR_MAX;
		std::uintptr_t FuncEnd = 0;

		for (auto OpCodeId = 0; OpCodeId < m_CodeBuffer->GetBufferSize(); OpCodeId++)
		{
			auto OpCode = OpCodes[OpCodeId];
			switch (OpCode)
			{/*we need to think in OpCodeId++, so we only want to add instr operands, the next iteration will
				have the next op code*/

			case rage::PUSH_CONST_U8:
				OpCodeId++;
				break;

			case rage::PUSH_CONST_U8_U8:
				OpCodeId += 2;
				break;

			case rage::PUSH_CONST_U8_U8_U8:
				OpCodeId += 3;
				break;

			case rage::PUSH_CONST_U32:
			case rage::PUSH_CONST_F:
				OpCodeId += 4;
				break;

			case rage::NATIVE:
				OpCodeId += 3;
				break;

			case rage::ENTER:

				if (FuncStart != UINTPTR_MAX)
				{
					if (auto Result = MapFunctionProto(FuncStart, FuncEnd + sizeof(rage::FuncEpilogue));
						Result != EzDecompilerStatus::NoError)
						return Result;
				}

				FuncStart = OpCodeId;

				OpCodeId += (OpCodes[OpCodeId + 4] + sizeof(rage::FuncPrologue) - 1); /*exclude the size of enter opcode*/
				break;

			case rage::LEAVE: /*since leaves are used to return we need to disguish functions by enter opcodes*/
				FuncEnd = OpCodeId;
				OpCodeId += sizeof(rage::FuncEpilogue) - 1; /*exclude the size of leave opcode*/
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
				OpCodeId++;
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
			case rage::J:
			case rage::JZ:
			case rage::IEQ_JZ:
			case rage::INE_JZ:
			case rage::IGT_JZ:
			case rage::IGE_JZ:
			case rage::ILT_JZ:
			case rage::ILE_JZ:
				OpCodeId += 2;
				break;

			case rage::CALL:
			case rage::GLOBAL_U24:
			case rage::GLOBAL_U24_LOAD:
			case rage::GLOBAL_U24_STORE:
			case rage::PUSH_CONST_U24:
				OpCodeId += 3;
				break;

			case rage::SWITCH: /*[OpCodes + 1] = switch cases*/
				OpCodeId += (1 + OpCodes[OpCodeId + 1] * sizeof(rage::FuncSwitchCase));
				break;

			case rage::TEXT_LABEL_ASSIGN_STRING:
			case rage::TEXT_LABEL_ASSIGN_INT:
			case rage::TEXT_LABEL_APPEND_STRING:
			case rage::TEXT_LABEL_APPEND_INT:
				OpCodeId++;
				break;


			default:
				if (OpCode > 126)
					return EzDecompilerStatus::UnexpectedOpCode;

				break;
			}
		}

		MapFunctionProto(FuncStart, m_CodeBuffer->GetBufferSize()); 
		/*we need to add specifically the last function because there won't be enter opcodes
		at the end of the script*/


		return EzDecompilerStatus::NoError;
	}

	EzFunction* EzDecompiler::GetFuncFromOffset(std::uintptr_t Offset)
	{
		for (auto& Func : m_Functions)
		{
			if ((Offset >= Func->GetFuncStartAddr()) && (Offset <= Func->GetFuncEndAddr()))
				return Func;
		}

		return nullptr; /*not a valid func*/
	}

	EzDecompilerStatus EzDecompiler::MapInstructionsFromFuncs()
	{
		m_MappedInstructions = 0;
		for (auto& Func : m_Functions)
		{
			if (auto Result = Func->MapInstructions(); Result != EzDecompilerStatus::NoError)
				return Result;

			m_MappedInstructions += Func->GetMappedInstructions();
		}

		return EzDecompilerStatus::NoError;
	}
	std::size_t EzDecompiler::GetMappedInstructions()
	{
		return m_MappedInstructions;
	}

	EzDecompilerStatus EzDecompiler::Disassemble()
	{
		m_DisassembledInstructions = 0;
		for (auto& Func : m_Functions)
		{
			if (auto Result = DisassembleFunc(Func); Result != EzDecompilerStatus::NoError)
				return Result;

			m_Assembly << m_LastAssembly.str();
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::DisassembleFunc(EzFunction* Func)
	{
		m_LastAssembly.str("");
		m_LastAssembly.clear(); /*reset m_LastAssembly*/

		auto& FuncInstrs = const_cast<std::vector<std::shared_ptr<EzInstruction>>&>
			(Func->GetInstructions());
		m_LastAssembly << "FSTART " << Func->GetFuncIndex() << "\n";

		for (std::uintptr_t i = 0; i < FuncInstrs.size(); i++)
		{
			//if (FuncInstrs[i]->GetInstructionId() == rage::RageInstr::NOP)
				//continue;

			if (Func->IsAddressASwitchLoc(FuncInstrs[i]->GetStartAddr()))
				m_LastAssembly << "\nswitch_loc_" << Func->GetSwitchLocIdByOffset(FuncInstrs[i]->GetStartAddr())
				<< ":\n";

			else if (Func->IsAddressALoc(FuncInstrs[i]->GetStartAddr()))
				m_LastAssembly << "\nloc_" << Func->GetLocIdByOffset(FuncInstrs[i]->GetStartAddr()) 
				               << ":\n";

			if (auto Result = DisassembleInstr(Func, FuncInstrs.data(), i);
					Result != EzDecompilerStatus::NoError)
				return Result;

			m_LastAssembly << "\n";
			m_DisassembledInstructions = i;
		}

		m_LastAssembly << "FEND\n";

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::DisassembleInstr(EzFunction* Func, 
		std::shared_ptr<EzInstruction>* Instructions, std::uintptr_t& InstrId)
	{
		auto& Instruction = Instructions[InstrId];
		auto OpCodes = m_CodeBuffer->GetData();
		static auto IsSwitch = false; /*used to know if we are handling any instruction inside
									  a switch, needed for JMPs right after them*/

		m_LastAssembly << std::hex << "0x" << Instruction->GetStartAddr() << ":\t";

		for (auto i = 0; i < Instruction->GetInstructionSize(); i++)
			m_LastAssembly << std::setw(2) << std::setfill('0') << std::hex
			               << std::uint16_t(OpCodes[Instruction->GetStartAddr() + i]) << " "; 
		/*print instruction opcodes*/
 
		m_LastAssembly << std::dec << "\t\t" << Instruction->Id2String(); /*print instruction string*/

		if (Instruction->IsCall())
		{
			auto Func = GetFuncFromOffset(Instruction->GetCallOffset());
			m_LastAssembly << "  func_" << Func->GetFuncIndex()
						   << "  //Func parameter count: "
						   << std::uint32_t(Func->GetPrologue()->m_ParemeterCount);
		}
		else if (Instruction->IsJump())
		{
			if (!IsSwitch)
				m_LastAssembly << "  loc_" << Func->GetLocIdByOffset(Instruction->GetJumpOffset());

			else
			{
				m_LastAssembly << "  switch_loc_" 
						       << Func->GetSwitchLocIdByOffset(Instruction->GetJumpOffset());
				IsSwitch = false;
			}
		}
		else if (Instruction->IsNativeCall())
		{
			m_LastAssembly << "  0x" << std::hex << m_NativeMap[Instruction->GetNativeIndex()]
						   << std::dec << "  //Native param count: "
						   << std::uint32_t(Instruction->GetNativeParamCount())
						   << ";Native return count: "
						   << std::uint32_t(Instruction->GetNativeReturnCount());
		}
		else if (Instruction->IsSwitch())
		{
			m_LastAssembly << "\n";

			InstrId++;
			while (Instructions[InstrId]->GetInstructionId() == rage::RageInstr::NOP)
				InstrId++;/*skip completely the switch & any posible nops after the it*/

			auto Cases = Instruction->GetSwitchCases();
			auto DefaultCaseJumpOff = Instructions[InstrId]->GetJumpOffset();
			auto DefaultCaseCodePos = GetInstructionPosFromAddr(DefaultCaseJumpOff) - 1;
			auto BreakJumpOff = UINTPTR_MAX;
			auto LastCaseCodePos = UINTPTR_MAX;

			auto IsBreakSameAsDefault = false;

			for (auto& Case : Cases)
			{
				m_LastAssembly << "\t\t case " << Case.first << "  JMP switch_loc_"
					<< Func->GetSwitchLocIdByOffset(Case.second);

				auto& LastCaseInstr = Instructions[LastCaseCodePos - 1];

				if (LastCaseCodePos != UINTPTR_MAX &&
					LastCaseInstr->GetInstructionId() == rage::RageInstr::J)
				{ /*if last instruction of the case is a JMP, the place where it jumps is the BREAK
					of the switch*/
					if (LastCaseInstr->GetJumpOffset() == DefaultCaseJumpOff)
						IsBreakSameAsDefault = true;
					/*break loc & default loc are the same, so default does not have a valid code path*/
					else
						BreakJumpOff = LastCaseInstr->GetJumpOffset();
					/*if default is used in this switch, then BREAK has it's own loc*/
				}

				LastCaseCodePos = GetInstructionPosFromAddr(Case.second);
				m_LastAssembly << "\n";
			}

			Func->AddSwitchLoc(BreakJumpOff);
			Func->AddSwitchLoc(DefaultCaseJumpOff);
			if (!IsBreakSameAsDefault)
			{
				m_LastAssembly << "\t\t default = switch_loc_"
					<< Func->GetSwitchLocIdByOffset(DefaultCaseJumpOff);

				m_LastAssembly << "\t\t //break = switch_loc_"
					<< Func->GetSwitchLocIdByOffset(BreakJumpOff);
			}
			else
			{
				m_LastAssembly << "\t\t //default = break = switch_loc_"
					<< Func->GetSwitchLocIdByOffset(DefaultCaseJumpOff);
			}
			
			m_LastAssembly << "\n";
			IsSwitch = true;

			return DisassembleInstr(Func, Instructions, InstrId); 
			/*print dissasembly of the JMP to the default case*/
		}
		else if (Instruction->GetOperandCount() > 0)
		{
			if (Instruction->IsFloat())
				m_LastAssembly << "  " << Instruction->GetOperandsF(OpCodes);

			else if (Instruction->IsSigned())
			
				m_LastAssembly << "  " << Instruction->GetOperandsI32(OpCodes);
			
			else
				m_LastAssembly << "  " << Instruction->GetOperandsU32(OpCodes);
			
		}

		return EzDecompilerStatus::NoError;
	}
	std::size_t EzDecompiler::GetDisassembledInstructions()
	{
		return m_DisassembledInstructions;
	}

	std::size_t EzDecompiler::GetInstructionPosFromAddr(std::uintptr_t Address)
	{
		for (auto Func : m_Functions)
		{
			if (Func->GetFuncStartAddr() <= Address && Func->GetFuncEndAddr() >= Address)
			{
				for (auto i = 0; i < Func->GetInstructions().size(); i++)
				{
					if (Func->GetInstructions()[i]->GetStartAddr() == Address)
						return i;
				}
			}
		}

		return SIZE_MAX;
	}

	EzDecompilerStatus EzDecompiler::MapFunctionProto(std::uintptr_t FuncStartAddr, std::uintptr_t FuncEndAddr)
	{
		auto BlockData = m_CodeBuffer->GetData();
		auto FuncPrologue = (rage::FuncPrologue*)&BlockData[FuncStartAddr];
		auto FuncEpilogue = (rage::FuncEpilogue*)&BlockData[FuncEndAddr - sizeof(rage::FuncEpilogue)];

		if (FuncPrologue->m_EnterOpCode != rage::RageInstr::ENTER)
			return EzDecompilerStatus::UnexpectedOpCode;

		else if (FuncEpilogue->m_LeaveOpCode != rage::RageInstr::LEAVE)
			return EzDecompilerStatus::UnexpectedOpCode;

		m_Functions.push_back(new EzFunction(m_CodeBuffer->GetData(), FuncStartAddr, FuncEndAddr,
			m_Functions.size(), FuncPrologue, FuncEpilogue));

		return EzDecompilerStatus::NoError;
	}

	std::ostringstream& EzDecompiler::GetAssembly()
	{
		return m_Assembly;
	}
	std::ostringstream& EzDecompiler::GetLastAssembly()
	{
		return m_LastAssembly;
	}

	void EzDecompiler::FreeDisassemblyBuffer()
	{
		m_Assembly.clear();
		m_Assembly.str("");

		m_LastAssembly.clear();
		m_LastAssembly.str("");
	}
}