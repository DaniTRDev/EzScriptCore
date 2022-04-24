#pragma once

#ifdef EZGLOBAL_INCLUDES /*use our own includes*/

#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <string_view>

#endif

namespace Ez
{
#define EZLOG_PRINTER(format, ...) printf(format, __VA_ARGS__) /*change this in case you want to change the logger*/

	enum class EzStatus
	{
		NotInitialized,
		InvalidFilePath,
		InvalidScriptName, /*the ptr passed to ParseHeader was nullptr*/
		InvalidFileContent, /*this is not a .ysc file*/
		InvalidFileHeader,
		CouldNotMapFunctions,
		CouldNotMapInstructions,
		Succeed /*everything is ok, ready to continue*/
	};

	struct EzScriptHeader /*the header of .ysc globals; directly ported to c++ from
						https://github.com/njames93/GTA-V-Script-Decompiler/blob/master/GTA%20V%20Script%20Decompiler/ScriptHeaders.cs*/
	{
		std::int32_t	m_Magic;
		std::int32_t	m_SubHeader;
		std::int32_t	m_CodeBlocksOffset;
		std::int32_t	m_GlobalsVersion;
		std::int32_t	m_CodeLength;
		std::int32_t	m_ParameterCount;
		std::int32_t	m_StaticsCount;
		std::int32_t	m_GlobalsCount;
		std::int32_t	m_NativesCount;
		std::int32_t	m_StaticsOffset;
		std::int32_t	m_GlobalsOffset;
		std::int32_t	m_NativesOffset;
		std::int32_t	m_Null1;
		std::int32_t	m_Null2;
		std::int32_t	m_NameHash;
		std::int32_t	m_Null3;
		std::int32_t	m_ScriptNameOffset;
		std::int32_t	m_StringsOffset;
		std::int32_t	m_StringsSize;
		std::int32_t	m_Null4;
		//Header End

		std::uint32_t	m_RSC7Offset;
		std::uint32_t* m_StringTableOffsets;
		std::uint32_t* m_CodeTableOffsets;
		std::uint32_t	m_StringBlocks;
		std::uint32_t	m_CodeBlocks;
		std::string		m_ScriptName;
		bool			m_RSC7;
	};
	struct EzScriptCodeBlock
	{
		std::unique_ptr<std::uint8_t[]> m_OpCodes; /*make sure to initialize this to 0*/
		std::size_t m_BlockSize{};
	};

	struct EzLocal
	{
		std::uintptr_t m_Index;
		std::int64_t m_Value;
	};

	enum EzInstructionIdx : std::uint8_t
	{
		Nop = 0,
		iAdd, //1
		iSub, //2
		iMult, //3
		iDiv, //4
		iMod, //5
		iNot, //6
		iNeg, //7
		iCmpEq, //8
		iCmpNe, //9
		iCmpGt, //10
		iCmpGe, //11
		iCmpLt, //12
		iCmpLe, //13
		fAdd, //14
		fSub, //15
		fMult, //16
		fDiv, //17
		fMod, //18
		fNeg, //19
		fCmpEq, //20
		fCmpNe, //21
		fCmpGt, //22
		fCmpGe, //23
		fCmpLt, //24
		fCmpLe, //25
		vAdd, //26
		vSub, //27
		vMult, //28
		vDiv, //29
		vNeg, //30
		And, //31
		Or, //32
		Xor, //33
		ItoF, //34
		FtoI, //35
		FtoV, //36
		iPushByte1, //37
		iPushByte2, //38
		iPushByte3, //39
		iPushInt, //40
		fPush, //41
		dup, //42
		pop, //43
		Native, //44
		Enter, //45
		Return, //46
		pGet, //47
		pSet, //48
		pPeekSet, //49
		ToStack, //50
		FromStack, //51
		pArray1, //52
		ArrayGet1, //53
		ArraySet1, //54
		pFrame1, //55
		GetFrame1, //56
		SetFrame1, //57
		pStatic1, //58 locals
		StaticGet1, //59 locals
		StaticSet1, //60 locals
		Add1, //61
		Mult1, //62
		pStructStack, //63
		pStruct1, //64
		GetStruct1, //65
		SetStruct1, //66
		iPushShort, //67
		Add2, //68
		Mult2, //69
		pStruct2, //70
		GetStruct2, //71
		SetStruct2, //72
		pArray2, //73
		ArrayGet2, //74
		ArraySet2, //75
		pFrame2, //76
		GetFrame2, //77
		SetFrame2, //78
		pStatic2, //79 locals
		StaticGet2, //80 locals
		StaticSet2, //81 locals
		pGlobal2, //82 globals
		GlobalGet2, //83 globals
		GlobalSet2, //84 globals
		Jump, //85
		JumpFalse, //86
		JumpNe, //87
		JumpEq, //88
		JumpLe, //89
		JumpLt, //90
		JumpGe, //91
		JumpGt, //92
		Call, //93
		pGlobal3, //94 globals
		GlobalGet3, //95 globals 
		GlobalSet3, //96 globals
		iPushI24, //97
		Switch, //98
		PushString, //99
		GetHash, //100
		StrCopy, //101
		ItoS, //102
		StrConCat, //103
		StrConCatInt, //104
		MemCopy, //105
		Catch, //106	 //No handling of these as Im unsure exactly how they work
		Throw, //107 //No script files in the game use these opcodes
		pCall, //108
		iPush_n1, //109
		iPush_0, //110
		iPush_1, //111
		iPush_2, //112
		iPush_3, //113
		iPush_4, //114
		iPush_5, //115
		iPush_6, //116
		iPush_7, //117
		fPush_n1, //118
		fPush_0, //119
		fPush_1, //120
		fPush_2, //121
		fPush_3, //122
		fPush_4, //123
		fPush_5, //124
		fPush_6, //125
		fPush_7 //126
	};
	struct EzInstruction
	{
		EzInstruction(std::uintptr_t StartAddr, std::uintptr_t EndAddr, EzInstructionIdx Id);

		std::uintptr_t m_StartAddr;
		std::uintptr_t m_EndAddr;

		EzInstructionIdx m_Id;
	};

	enum class EzFuncState
	{
		None,
		PseudoDecompiled,
		Decompiled
	};
	struct EzFunc
	{
		EzFunc(std::uintptr_t Start, std::uintptr_t End);

		std::uintptr_t m_Start; /*offset relative to the code block start*/
		std::uintptr_t m_End;

		std::vector<EzInstruction> m_Instructions;
		EzFuncState m_State;
	};

	struct EzScriptFile
	{
		enum EzOperations : std::uint32_t /*tell what we have done at the momment*/
		{
			ParsedHeader = (1 << 0),
			MappedLocals = (1 << 1),
			MappedBlocks = (1 << 2),
			MappedFunctions = (1 << 3),
			MappedFuncsInstructions = (1 << 5),
			DecompiledFuncsInstructions = (1 << 6),
		};

		EzScriptFile(std::filesystem::path Path);

		std::ifstream m_Stream;

		EzStatus m_Status;
		std::uint32_t m_FinishedOperations;

		EzScriptHeader m_Header;
		EzScriptCodeBlock m_CodeBlock;

		std::vector<EzLocal> m_Locals;
		std::vector<std::uint32_t> m_Globals;

		std::vector<EzFunc> m_Functions;

		bool ParseHeader(const char* ScriptName);

		bool MapLocals();
		bool MapGlobals();

		bool MapCodeBlocks();
		bool MapFunctions();

		bool MapFuncsInstructions();
		bool DecompileFuncsInstructions();
	};

#pragma region BitHelpersH
	template<typename T>
	std::int16_t Bits2U16(T First, T Second);

	template<>
	std::int16_t Bits2U16(std::uint8_t First, std::uint8_t Second);

	template<>
	std::int16_t Bits2U16(std::int8_t First, std::int8_t Second);

	template<typename T>
	std::int32_t BitsToU32(T First, T Second, T Third, T Fourth); /*int32 supports both int32 & uint32 */

	template<>
	std::int32_t BitsToU32(std::uint8_t First, std::uint8_t Second, std::uint8_t Third, std::uint8_t Fourth);

	template<>
	std::int32_t BitsToU32(std::int8_t First, std::int8_t Second, std::int8_t Third, std::int8_t Fourth);

#pragma endregion

#ifdef EZGLOBAL_IMPL
#pragma region StreamHelpers
	template <typename T>
	T Read(std::istream& Stream)
	{
		T DummyVar;
		Stream.read((char*)&DummyVar, sizeof(T));

		return DummyVar;
	}
	std::uint32_t ReadPtr(std::istream& Stream)
	{
		auto EncryptedVal = Read<std::int32_t>(Stream);
		return EncryptedVal & 0xFFFFFF;
	}

	void IncrementStreamPtr(std::istream& Stream, std::size_t Size)
	{
		Stream.seekg(std::size_t(Stream.tellg()) + std::size_t(4), Stream.beg);
	}
#pragma endregion

#pragma region BitHelpers

	template<>
	std::int16_t Bits2U16(std::uint8_t First, std::uint8_t Second)
	{
		return std::uint16_t(First << 8 | Second);
	}

	template<>
	std::int16_t Bits2U16(std::int8_t First, std::int8_t Second)
	{
		return std::uint16_t(First << 8 | Second);
	}

	template<>
	std::int32_t BitsToU32(std::uint8_t First, std::uint8_t Second, std::uint8_t Third, std::uint8_t Fourth)
	{
		if (Fourth)
			return ((First << 24) | (Second << 16) | (Third << 8) | (Fourth));

		return ((First << 16) | (Second << 8) | (Third));
	}

	template<>
	std::int32_t BitsToU32(std::int8_t First, std::int8_t Second, std::int8_t Third, std::int8_t Fourth)
	{
		if (Fourth)
			return ((First << 24) | (Second << 16) | (Third << 8) | (Fourth));

		return ((First << 16) | (Second << 8) | (Third));
	}

#pragma endregion

	std::uint32_t Joaat(std::string_view Str)
	{
		std::uint32_t Hash = 0;
		for (auto Char : Str)
		{
			Hash += Char;
			Hash += (Hash << 10);
			Hash ^= (Hash >> 6);
		}
		Hash += (Hash << 3);
		Hash ^= (Hash >> 11);
		Hash += (Hash << 15);

		return Hash;
	}

	EzScriptFile::EzScriptFile(std::filesystem::path Path) : m_Stream(Path.string(), std::ifstream::in | std::ifstream::binary),
		m_Status(EzStatus::NotInitialized)
	{
		if (!m_Stream.is_open())
			m_Status = EzStatus::InvalidFilePath;

	}

	bool EzScriptFile::ParseHeader(const char* ScriptName) /*scriptname is just to check we parsed correctly*/
	{
		if (m_Status != EzStatus::NotInitialized) /*m_Status will be set to NotInitialized on constructor*/
			return false;
		else if (!ScriptName)
		{
			m_Status = EzStatus::InvalidScriptName;
			return false;
		}

		m_Stream.seekg(0, m_Stream.beg);
		m_Header.m_Magic = Read<std::uint32_t>(m_Stream);

		m_Header.m_RSC7Offset = (m_Header.m_Magic == 0x37435352) ? 0x10 : 0x0;
		m_Stream.seekg(m_Header.m_RSC7Offset, m_Stream.beg);

		m_Header.m_Magic = Read<std::int32_t>(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_SubHeader = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_CodeBlocksOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_GlobalsVersion = Read<std::int32_t>(m_Stream);
		m_Header.m_CodeLength = Read<std::int32_t>(m_Stream);
		m_Header.m_ParameterCount = Read<std::int32_t>(m_Stream);
		m_Header.m_StaticsCount = Read<std::int32_t>(m_Stream);
		m_Header.m_GlobalsCount = Read<std::int32_t>(m_Stream);
		m_Header.m_NativesCount = Read<std::int32_t>(m_Stream);

		m_Header.m_StaticsOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_GlobalsOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_NativesOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_Null1 = Read<std::int32_t>(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_Null2 = Read<std::int32_t>(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_NameHash = Read<std::int32_t>(m_Stream);
		m_Header.m_Null3 = Read<std::int32_t>(m_Stream);

		m_Header.m_ScriptNameOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_StringsOffset = ReadPtr(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_StringsSize = Read<std::int32_t>(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_Null4 = Read<std::int32_t>(m_Stream);
		IncrementStreamPtr(m_Stream, 4);

		m_Header.m_StringBlocks = (m_Header.m_StringsSize + 0x3FFF) >> 14;
		m_Header.m_CodeBlocks = (m_Header.m_CodeLength + 0x3FFF) >> 14;

		/*m_Header.m_StringTableOffsets = new std::uint32_t[m_Header.m_StringBlocks];
		m_Stream.seekg(m_Header.m_StringsOffset + m_Header.m_RSC7Offset, Stream.beg);
		for (std::uint32_t i = 0; i < m_Header.m_StringBlocks; i++) //we don't need this at all, I'm keeping zorg's code in case anyone
		{										 //wants to create their custom decompiler in c++ with this shit lol
			m_Header.m_StringTableOffsets[i] = ReadPtr(m_Stream) + m_Header.m_RSC7Offset;
			IncrementStreamPtr(m_Stream, 4);
		}*/


		m_Header.m_CodeTableOffsets = new std::uint32_t[m_Header.m_CodeBlocks];
		m_Stream.seekg(m_Header.m_CodeBlocksOffset + m_Header.m_RSC7Offset, m_Stream.beg);
		for (std::uint32_t i = 0; i < m_Header.m_CodeBlocks; i++)
		{
			m_Header.m_CodeTableOffsets[i] = ReadPtr(m_Stream) + m_Header.m_RSC7Offset;
			IncrementStreamPtr(m_Stream, 4);
		}

		m_Stream.seekg(m_Header.m_ScriptNameOffset + m_Header.m_RSC7Offset, m_Stream.beg);

		auto CurrentByte = Read<std::int8_t>(m_Stream);
		m_Header.m_ScriptName = "";

		while (CurrentByte != 0 && CurrentByte != -1)
		{
			m_Header.m_ScriptName += (char)CurrentByte;
			CurrentByte = Read<std::int8_t>(m_Stream);

			if (m_Header.m_ScriptName.length() > 100) /*we've entered in an infinite loop because an error in the parsing,
												this is not an script file*/
			{
				m_Status = EzStatus::InvalidFileContent;
				return false;
			}

		}

		if (m_Header.m_NameHash != Joaat(ScriptName))
		{
			m_Status = EzStatus::InvalidFileContent;
			return false;
		}

		EZLOG_PRINTER("Parsed script %s! Script hash: %x\n\n///////////////\n\n", m_Header.m_ScriptName.c_str(),
			m_Header.m_NameHash);

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::ParsedHeader;

		return true;
	}

	bool EzScriptFile::MapLocals()
	{
		if (m_Status != EzStatus::Succeed)
		{
			EZLOG_PRINTER("There was an error in the previous calls, check it!\n");
			return false;
		}
		else if (!(m_FinishedOperations & EzOperations::ParsedHeader))
		{
			EZLOG_PRINTER("You must call ParseHeader first!\n");
			return false;
		}

		m_Stream.seekg(m_Header.m_StaticsOffset + m_Header.m_RSC7Offset);

		std::vector<EzLocal> TempLocals;

		for (auto i = 0; i < m_Header.m_StaticsCount; i++)
			TempLocals.push_back(EzLocal{ TempLocals.size(), Read<std::int64_t>(m_Stream) });

		EZLOG_PRINTER("Mapped %llu locals & script params!\n", TempLocals.size());

		/*we need to discard script start params statics*/
		auto ScriptParamStart = TempLocals.size() - m_Header.m_ParameterCount;
		for (auto i = 0; i < TempLocals.size(); i++)
		{
			if (TempLocals[i].m_Index < ScriptParamStart)
				m_Locals.push_back(TempLocals[i]);
		}
		EZLOG_PRINTER("Mapped %llu locals!\n\n///////////////\n\n", m_Locals.size());

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::MappedLocals;

		TempLocals.clear();

		return true;
	}

	bool EzScriptFile::MapCodeBlocks()
	{
		if (m_Status != EzStatus::Succeed)
		{
			EZLOG_PRINTER("There was an error in the previous calls, check it!\n");
			return false;
		}
		else if (!(m_FinishedOperations & EzOperations::ParsedHeader))
		{
			EZLOG_PRINTER("You must call ParseHeader first!\n");
			return false;
		}

		auto TotalBlockSize = std::uintptr_t(m_Header.m_CodeBlocks - 1) * std::uintptr_t(0x4000);
		if ((m_Header.m_CodeBlocks * 0x4000) > m_Header.m_CodeLength)
		{
			auto RemainSize = (m_Header.m_CodeBlocks * 0x4000) - (m_Header.m_CodeLength);
			TotalBlockSize += (0x4000 - RemainSize);
		}

		m_CodeBlock.m_OpCodes = std::make_unique<std::uint8_t[]>(TotalBlockSize);
		m_CodeBlock.m_BlockSize = TotalBlockSize;

		for (std::uint32_t i = 0; i < m_Header.m_CodeBlocks; i++)
		{
			auto TableSize = (std::int32_t((i + 1) * 0x4000) >= m_Header.m_CodeLength) ? m_Header.m_CodeLength % 0x4000 : 0x4000;
			
			m_Stream.seekg(m_Header.m_CodeTableOffsets[i], m_Stream.beg);
			m_Stream.read((char*)m_CodeBlock.m_OpCodes.get(), 
				(std::uintptr_t(i) * std::uintptr_t(0x4000)));
		}

		EZLOG_PRINTER("Mapped %d blocks -> Merged in a single block with size: 0x%zx \n\n///////////////\n\n", m_Header.m_CodeBlocks, m_CodeBlock.m_BlockSize);

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::MappedBlocks;

		return true;
	}
	bool EzScriptFile::MapFunctions()
	{
		if (m_Status != EzStatus::Succeed)
		{
			EZLOG_PRINTER("There was an error in the previous calls, check it!\n");
			return false;
		}
		else if (!(m_FinishedOperations & EzOperations::MappedBlocks))
		{
			EZLOG_PRINTER("You must call MapCodeBlocks first!\n");
			return false;
		}

		auto OpCodes = m_CodeBlock.m_OpCodes.get();
		std::size_t ReturnAddres = -3;

		std::vector<EzFunc> FuncEnters;

		for (std::size_t OpCodeId = 0; OpCodeId < m_CodeBlock.m_BlockSize; OpCodeId++)
		{
			switch (OpCodes[OpCodeId])
			{
			case 37: OpCodeId++; break;
			case 38: OpCodeId += 2; break;
			case 39: OpCodeId += 3; break;
			case 40:
			case 41: OpCodeId += 4; break;
			case 44: OpCodeId += 3; break;
			case 45: FuncEnters.push_back(EzFunc(ReturnAddres + 3, OpCodeId));
				OpCodeId += (OpCodes[OpCodeId + 4] + 4); break;

			case 46: ReturnAddres = OpCodeId; OpCodeId += 2; break;
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 64:
			case 65:
			case 66: OpCodeId++; break;
			case 67:
			case 68:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
			case 90:
			case 91:
			case 92: OpCodeId += 2; break;
			case 93:
			case 94:
			case 95:
			case 96:
			case 97: OpCodeId += 3; break;
			case 98: OpCodeId += (1 + OpCodes[OpCodeId + 1] * 6); break;
			case 101:
			case 102:
			case 103:
			case 104: OpCodeId++; break;
			}
		}

		EZLOG_PRINTER("Mapped %zu function enters!\n", FuncEnters.size());
		if (FuncEnters.size() == 0)
		{
			m_Status = EzStatus::CouldNotMapFunctions;
			return false;
		}

		for (std::size_t i = 0; i < FuncEnters.size(); i++)
		{
			if (i == FuncEnters.size() - 1)
			{
				m_Functions.push_back(EzFunc(FuncEnters[i].m_End, m_CodeBlock.m_BlockSize));
				break;
			}

			auto StartAddr = FuncEnters[i].m_End;
			auto EndAddr = FuncEnters[i + 1].m_Start;

			if (OpCodes[StartAddr] != 45 || OpCodes[EndAddr - 3] != 46)
			{
				m_Status = EzStatus::CouldNotMapFunctions;
				return false;
			}

			m_Functions.push_back(EzFunc(StartAddr, EndAddr));
		}

		EZLOG_PRINTER("Mapped %zu functions!\n\n///////////////\n\n", m_Functions.size());

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::MappedFunctions;

		FuncEnters.clear();
		return true;
	}

	void MapInstrFromDup(EzFunc& Func, std::uint8_t* OpCodes, std::uintptr_t& OpCodeId, EzInstructionIdx OpCode)
	{
		auto CurrentOffset = 1;
		for (;;CurrentOffset++)
		{

			if (OpCodes[OpCodeId + CurrentOffset] == EzInstructionIdx::Nop)
				continue;

			if (OpCodes[OpCodeId + CurrentOffset] == EzInstructionIdx::JumpFalse)
			{
				OpCodeId += std::uintptr_t(CurrentOffset + 2);
				break;
			}

			if (OpCodes[OpCodeId + CurrentOffset] == EzInstructionIdx::iNot)
				continue;

			Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + CurrentOffset, 
				static_cast<EzInstructionIdx>(OpCodes[OpCodeId + CurrentOffset])));
			break;
		}
}
	bool EzScriptFile::MapFuncsInstructions() /*will only map GLOBALS / LOCALS related instructions*/
	{
		if (m_Status != EzStatus::Succeed)
		{
			EZLOG_PRINTER("There was an error in the previous calls, check it!\n");
			return false;
		}
		else if (!(m_FinishedOperations & EzOperations::MappedFunctions))
		{
			EZLOG_PRINTER("You must call MapFunctions first!\n");
			return false;
		}

		std::size_t InstructionCount = 0;

		for (auto& Func : m_Functions)
		{
			auto StartOff = Func.m_Start + m_CodeBlock.m_OpCodes[Func.m_Start + 4] + 5;
			auto EndOff = Func.m_End;

			for(auto OpCodeId = StartOff; OpCodeId < EndOff; OpCodeId++) /*startOff + 1 to skip the func enter opcode*/
			{
				auto CurOpCode = static_cast<EzInstructionIdx>(m_CodeBlock.m_OpCodes[OpCodeId]);
				switch (CurOpCode)
				{
					//		case 0: if (addnop) AddInstruction(curoff, new HLInstruction((byte)0, curoff)); break;
				case EzInstructionIdx::iPushByte1:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 1, CurOpCode));
					OpCodeId++;
					break;

				case EzInstructionIdx::iPushByte2:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 2, CurOpCode));
					OpCodeId += 2;
					break;

				case EzInstructionIdx::iPushByte3:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 3, CurOpCode));
					OpCodeId += 3;
					break;

				case EzInstructionIdx::iPushInt:
				case EzInstructionIdx::fPush:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 4, CurOpCode));
					OpCodeId += 4;
					break;

				case EzInstructionIdx::dup: //Because of how rockstar codes and/or conditionals, its neater to detect dups
					//and only add them if they are not used for conditionals
					MapInstrFromDup(Func, m_CodeBlock.m_OpCodes.get(), OpCodeId, CurOpCode);
					break;

				case EzInstructionIdx::Native:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 3, CurOpCode));
					OpCodeId += 3;
					break;

				case EzInstructionIdx::Enter:
					EZLOG_PRINTER("Unexpected enter opcode %d at offset: %zx\n", static_cast<std::uint8_t>(CurOpCode), OpCodeId);
					m_Status = EzStatus::CouldNotMapInstructions;
					return false;

				case EzInstructionIdx::Return:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 2, CurOpCode));
					OpCodeId += 2;
					break;

				case EzInstructionIdx::pArray1:
				case EzInstructionIdx::ArrayGet1:
				case EzInstructionIdx::ArraySet1:
				case EzInstructionIdx::pFrame1:
				case EzInstructionIdx::GetFrame1:
				case EzInstructionIdx::SetFrame1:
				case EzInstructionIdx::pStatic1:
				case EzInstructionIdx::StaticGet1:
				case EzInstructionIdx::StaticSet1:
				case EzInstructionIdx::Add1:
				case EzInstructionIdx::Mult1:
				case EzInstructionIdx::pStruct1:
				case EzInstructionIdx::pStructStack:
				case EzInstructionIdx::SetStruct1:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 1, CurOpCode));
					OpCodeId++;
					break;

				case EzInstructionIdx::iPushShort:
				case EzInstructionIdx::Add2:
				case EzInstructionIdx::Mult2:
				case EzInstructionIdx::pStruct2:
				case EzInstructionIdx::GetStruct2:
				case EzInstructionIdx::SetStruct2:
				case EzInstructionIdx::pArray2:
				case EzInstructionIdx::ArrayGet2:
				case EzInstructionIdx::ArraySet2:
				case EzInstructionIdx::pFrame2:
				case EzInstructionIdx::GetFrame2:
				case EzInstructionIdx::SetFrame2:
				case EzInstructionIdx::pStatic2:
				case EzInstructionIdx::StaticGet2:
				case EzInstructionIdx::StaticSet2:
				case EzInstructionIdx::pGlobal2:
				case EzInstructionIdx::GlobalGet2:
				case EzInstructionIdx::GlobalSet2:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 2, CurOpCode));
					OpCodeId += 2;
					break;

				case EzInstructionIdx::Jump:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 2, CurOpCode));
					OpCodeId += 2;
					break;

				case EzInstructionIdx::JumpFalse:
				case EzInstructionIdx::JumpNe:
				case EzInstructionIdx::JumpEq:
				case EzInstructionIdx::JumpLe:
				case EzInstructionIdx::JumpLt:
				case EzInstructionIdx::JumpGe:
				case EzInstructionIdx::JumpGt:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 2, CurOpCode));
					OpCodeId += 2;
					break;

				case EzInstructionIdx::Call:
				case EzInstructionIdx::pGlobal3:
				case EzInstructionIdx::GlobalGet3:
				case EzInstructionIdx::GlobalSet3:
				case EzInstructionIdx::iPushI24:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 3, CurOpCode));
					OpCodeId += 3;
					break;

				case EzInstructionIdx::Switch:
				{
					auto NumItems = std::uint32_t(m_CodeBlock.m_OpCodes[OpCodeId + 1]);
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + NumItems * 6 + 1, CurOpCode));
					OpCodeId += NumItems * 6 + 1;
				}	break;

				case EzInstructionIdx::StrCopy:
				case EzInstructionIdx::ItoS:
				case EzInstructionIdx::StrConCat:
				case EzInstructionIdx::StrConCatInt:
					Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId + 1, CurOpCode));
					OpCodeId++;
					break;

				default:
					if (CurOpCode > 126)
					{
						EZLOG_PRINTER("Unexpected opcode %d at offset: %zx\n", static_cast<std::uint8_t>(CurOpCode), OpCodeId);
						m_Status = EzStatus::CouldNotMapInstructions;
						return false;
					}
						Func.m_Instructions.push_back(EzInstruction(OpCodeId, OpCodeId, CurOpCode));
					break;
				}

			}

			InstructionCount += Func.m_Instructions.size();
		}

		EZLOG_PRINTER("Mapped %zu instructions!\n\n///////////////\n\n", InstructionCount);

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::MappedFuncsInstructions;

		return true;
	}
	bool EzScriptFile::DecompileFuncsInstructions()
	{
		if (m_Status != EzStatus::Succeed)
		{
			EZLOG_PRINTER("There was an error in the previous calls, check it!\n");
			return false;
		}
		else if (!(m_FinishedOperations & EzOperations::MappedFuncsInstructions))
		{
			EZLOG_PRINTER("You must call MapFuncsInstructions first!\n");
			return false;
		}

		auto OpCodes = m_CodeBlock.m_OpCodes.get();
		std::size_t GlobalsReferences = 0;

		for (auto& Func : m_Functions)
		{
			for (auto& Instr : Func.m_Instructions)
			{
				if (Instr.m_Id == EzInstructionIdx::GlobalGet2 || Instr.m_Id == EzInstructionIdx::GlobalGet3
					|| Instr.m_Id == EzInstructionIdx::GlobalSet2 || Instr.m_Id == EzInstructionIdx::GlobalSet3
					|| Instr.m_Id == EzInstructionIdx::pGlobal2 || Instr.m_Id == EzInstructionIdx::pGlobal3)
				{ /*globals*/
					auto NumOperands = Instr.m_EndAddr - Instr.m_StartAddr;
					auto Index = 0;

					switch (NumOperands)
					{
					case 1:
						Index = std::uint32_t(OpCodes[Instr.m_StartAddr + NumOperands]);
						break;

					case 2:
						Index = std::uint32_t(std::uint16_t(OpCodes[Instr.m_StartAddr + NumOperands] << 8
							| OpCodes[Instr.m_StartAddr + 1]));
						break;

					case 3:
						Index = std::uint32_t(OpCodes[Instr.m_StartAddr + NumOperands] << 16 |
							OpCodes[Instr.m_StartAddr + 2] << 8 | OpCodes[Instr.m_StartAddr + 1]);
						break;

					case 4:
						Index = std::uint32_t(OpCodes[Instr.m_StartAddr + NumOperands] << 32
							| OpCodes[Instr.m_StartAddr + 3] << 16 | OpCodes[Instr.m_StartAddr + 2] << 8
							| OpCodes[Instr.m_StartAddr + 1]);
						break;
					}

					if (auto It = std::find(m_Globals.begin(), m_Globals.end(), Index); It == m_Globals.end())
						m_Globals.push_back(Index);
					/*we need to do that check above because a global can be referenced multiple times*/
					GlobalsReferences++;
				}
			}
		}

		EZLOG_PRINTER("Found %zu globals! \nFound %zu references to globals!\n\n///////////////\n\n", 
			m_Globals.size(), GlobalsReferences);

		m_Status = EzStatus::Succeed;
		m_FinishedOperations |= EzOperations::DecompiledFuncsInstructions;

		return true;
	}

	EzInstruction::EzInstruction(std::uintptr_t StartAddr, std::uintptr_t EndAddr, EzInstructionIdx Id) : 
		m_StartAddr(StartAddr), m_EndAddr(EndAddr), m_Id(Id)
	{}

	EzFunc::EzFunc(std::uintptr_t Start, std::uintptr_t End) : m_Start(Start), m_End(End), m_State(EzFuncState::None)
	{}
}
#endif