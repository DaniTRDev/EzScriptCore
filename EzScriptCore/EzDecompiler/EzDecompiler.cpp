#include "../pch.h"

namespace Ez
{
	EzDecompiler::EzDecompiler(EzSrcProgram* Script) : m_ScriptBuffer(std::move(Script->m_ScriptBuff)),
		m_OpCodeId(0), m_Script(Script), m_DecompiledInstructions(0)
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

	const std::size_t EzDecompiler::GetBlocksNumber()
	{
		return m_Script->m_CodeBlocks;
	}
	const std::size_t EzDecompiler::GetBlockSize()
	{
		return m_CodeBlock->GetBufferSize();
	}

	const std::vector<EzFunction*>& EzDecompiler::GetFunctions()
	{
		return m_Functions;
	}

	std::size_t EzDecompiler::GetDecompiledInstructions()
	{
		return m_DecompiledInstructions;
	}

	EzDecompilerStatus EzDecompiler::MapLocals()
	{
		if (!m_Script->m_ConsoleH && !m_Script->m_PCH)
			return EzDecompilerStatus::InvalidScriptHeader;

		if (m_Script->m_IsConsole)
			return MapLocals(m_Script->m_ConsoleH->m_StaticsOffset.m_Offset,
				m_Script->m_ConsoleH->m_StaticsCount, m_Script->m_ConsoleH->m_ParameterCount);

		return MapLocals(m_Script->m_PCH->m_StaticsOffset.m_Offset,
			m_Script->m_PCH->m_StaticsCount, m_Script->m_PCH->m_ParameterCount);
	}
	EzDecompilerStatus EzDecompiler::MapStrings()
	{
		if (!m_Script->m_ConsoleH && !m_Script->m_PCH)
			return EzDecompilerStatus::InvalidScriptHeader;

		else if (m_Script->m_IsConsole)
			return MapStrings(m_Script->m_ConsoleH->m_StringsSize);

		return MapStrings(m_Script->m_PCH->m_StringsSize);
	}
	EzDecompilerStatus EzDecompiler::MapCodeBlocks()
	{
		if (!m_Script->m_ConsoleH && !m_Script->m_PCH)
			return EzDecompilerStatus::InvalidScriptHeader;

		else if (m_Script->m_IsConsole)
			return MapCodeBlocks(m_Script->m_ConsoleH->m_CodeLength);

		return MapCodeBlocks(m_Script->m_PCH->m_CodeLength);
	}

	EzDecompilerStatus EzDecompiler::MapFunctions()
	{
		if (!m_CodeBlock)
			return EzDecompilerStatus::CodeBlocksNotMapped;

		auto OpCodes = m_CodeBlock->GetData();
		std::uintptr_t FuncStart = 0;

		/*I know there are opcodes that are useless in the switch because we break on them
			but I just want to keep them there in case something changes in the future; 
		  Also, everytime I need to walk the opcodes I'll just copy this switch and modify how to handle cases*/
		for (auto OpCodeId = 0; OpCodeId < m_CodeBlock->GetBufferSize(); OpCodeId++)
		{
			auto OpCode = OpCodes[OpCodeId];
			switch (OpCode)
			{/*we need to think in OpCodeId++, so we only want to add the func size, the next iteration will
				have the next op code*/
			case rage::NOP: /*instr size == 1 so it will be skipped in the next iteration of this for*/
			case rage::IADD:
			case rage::ISUB:
			case rage::IMUL:
			case rage::IDIV:
			case rage::IMOD:
			case rage::INOT:
			case rage::INEG:
			case rage::IEQ:
			case rage::INE:
			case rage::IGT:
			case rage::IGE:
			case rage::ILT:
			case rage::ILE:
			case rage::FADD:
			case rage::FSUB:
			case rage::FMUL:
			case rage::FDIV:
			case rage::FMOD:
			case rage::FNEG:
			case rage::FEQ:
			case rage::FNE:
			case rage::FGT:
			case rage::FGE:
			case rage::FLT:
			case rage::FLE:
			case rage::VADD:
			case rage::VSUB:
			case rage::VMUL:
			case rage::VDIV:
			case rage::VNEG:
			case rage::IAND:
			case rage::IOR:
			case rage::IXOR:
			case rage::I2F:
			case rage::F2I:
			case rage::F2V:
				break;

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

			case rage::DUP:
			case rage::DROP: /*instr size == 1*/
				break;

			case rage::NATIVE:
				OpCodeId += 3;
				break;

			case rage::ENTER:
				FuncStart = OpCodeId;
				OpCodeId += (OpCodes[OpCodeId + 4] + sizeof(rage::FuncPrologue) - 1); /*exclude the size of enter opcode*/
				break;

			case rage::LEAVE:
				MapFunctionProto(FuncStart, OpCodeId + sizeof(rage::FuncEpilogue));
				OpCodeId += sizeof(rage::FuncEpilogue) - 1; /*exclude the size of leave opcode*/
				break;

			case rage::LOAD:
			case rage::STORE:
			case rage::STORE_REV:
			case rage::LOAD_N:
			case rage::STORE_N:
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

			case rage::STRING:
			case rage::STRINGHASH: /*instr size == 1*/
				break;

			case rage::TEXT_LABEL_ASSIGN_STRING:
			case rage::TEXT_LABEL_ASSIGN_INT:
			case rage::TEXT_LABEL_APPEND_STRING:
			case rage::TEXT_LABEL_APPEND_INT:
				OpCodeId++;
				break;
			}
		}

		return EzDecompilerStatus::NoError;
	}

	EzDecompilerStatus EzDecompiler::PreDecompileFunctions()
	{
		m_DecompiledInstructions = 0;

		for (auto& Func : m_Functions)
		{
			if (auto Result = Func->PreDecompile(m_Script->m_IsConsole); Result != EzDecompilerStatus::NoError)
				return Result;

			m_DecompiledInstructions += Func->GetDecompiledInstrNum();
		}

		return EzDecompilerStatus::NoError;
	}

	EzDecompilerStatus EzDecompiler::MapLocals(std::int32_t StaticsOffset, std::int32_t StaticsCount,
		std::int32_t ParameterCount)
	{
		m_ScriptBuffer->SetPos(StaticsOffset + m_Script->m_RSC7Offset);

		for (auto i = 0; i < StaticsCount - ParameterCount; i++)
		{
			std::int64_t Value = 0;
			m_ScriptBuffer->Read((char*)&Value, sizeof(Value));

			if (m_Script->m_IsConsole)
				Value = Endians::Reverse32(std::uint32_t(Value));

			m_Locals.push_back(rage::EzLocal{ std::uint32_t(m_Locals.size()), Value });
		}

		for (auto i = StaticsCount - ParameterCount; i < StaticsCount; i++)
		{
			std::int64_t Value = 0;
			m_ScriptBuffer->Read((char*)&Value, sizeof(Value));

			if (m_Script->m_IsConsole)
				Value = Endians::Reverse32(std::uint32_t(Value));

			m_ScriptParams.push_back(rage::EzScriptParam{ std::uint32_t(m_Locals.size()), Value });
		}

		return EzDecompilerStatus::NoError;
	}

	EzDecompilerStatus EzDecompiler::MapStrings(std::int32_t StringsSize)
	{
		/*first map string tables and merge them in a single table*/
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

	EzDecompilerStatus EzDecompiler::MapCodeBlocks(std::int32_t CodeLenght)
	{
		m_CodeBlock = std::make_unique<EzBuffer>();
		m_CodeBlock->AllocateBuffer(CodeLenght);

		for (auto i = 0; i < m_Script->m_CodeBlocks; i++)
		{
			auto TableSize = (std::int32_t((i + 1) * 0x4000) >= CodeLenght) ? CodeLenght % 0x4000 : 0x4000;
			auto Block = std::make_unique<std::uint8_t[]>(TableSize);

			m_ScriptBuffer->SetPos(m_Script->m_CodeTableOffsets[i]);
			m_ScriptBuffer->Read(Block, TableSize);
			m_CodeBlock->Write(std::move(Block), TableSize);
		}

		return EzDecompilerStatus::NoError;
	}
	EzDecompilerStatus EzDecompiler::MapFunctionProto(std::uintptr_t FuncStartAddr, std::uintptr_t FuncEndAddr)
	{
		auto BlockData = m_CodeBlock->GetData();
		auto FuncPrologue = (rage::FuncPrologue*)&BlockData[FuncStartAddr];
		auto FuncEpilogue = (rage::FuncEpilogue*)&BlockData[FuncEndAddr - sizeof(rage::FuncEpilogue)];

		if (m_Script->m_IsConsole)
			FuncPrologue->m_VariableCount = Endians::Reverse16(FuncPrologue->m_VariableCount);

		if (FuncPrologue->m_EnterOpCode != rage::RageInstr::ENTER)
			return EzDecompilerStatus::UnexpectedOpCode;

		else if (FuncEpilogue->m_LeaveOpCode != rage::RageInstr::LEAVE)
			return EzDecompilerStatus::UnexpectedOpCode;

		m_Functions.push_back(new EzFunction(m_CodeBlock->GetData(), FuncStartAddr, FuncEndAddr,
			FuncPrologue, FuncEpilogue));

		return EzDecompilerStatus::NoError;
	}
}