#include "pch.h"

namespace Ez
{
	namespace rage
	{
		std::uint32_t Joaat(std::string Str)
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

		void Ensure24Bits(PCOffsetEntry& Entry)
		{
			Entry.m_Offset = (Entry.m_Offset & 0xFFFFFF);
		}
		void Ensure24Bits(ConsoleOffsetEntry& Entry)
		{
			/*swap from big endian to little endian and then decrypt*/
			Entry.m_Offset = Endians::Reverse32(Entry.m_Offset) & 0xFFFFFF;
		}
	}


	EzSrcProgram::EzSrcProgram() : m_ConsoleH(nullptr), m_PCH(nullptr), m_IsConsole(false), m_RSC7Offset(0), m_StringTableOffsets(nullptr),
		m_CodeTableOffsets(nullptr), m_StringBlocks(0), m_CodeBlocks(0), m_ExpectedScriptName(""), m_ScriptName(""), 
		m_RSC7(0), m_ScriptBuff(std::make_unique<EzBuffer>())
	{
	}

	EzScriptStatus EzSrcProgram::ParseHeader(std::istream& Stream, std::string ExpectedScriptName, bool IsConsole)
	{
		if (m_ConsoleH || m_PCH)
			return EzScriptStatus::AlreadyParsed;

		if (auto Result = m_ScriptBuff->FromStream(Stream); Result != EzBufferStatus::NoError)
			throw std::runtime_error("Error while parsing script header!");

		std::int32_t Magic = 0;
		m_ScriptBuff->Read((char*)&Magic, sizeof(Magic));

		if (m_IsConsole && Magic == 0x52534337)
			m_RSC7Offset = 0x10;

		else if (!m_IsConsole && Magic == 0x37435352)
			m_RSC7Offset = 0x10;

		m_RSC7 = (m_RSC7Offset != 0);
		m_ExpectedScriptName = ExpectedScriptName;

		m_ScriptBuff->SetPos(m_RSC7Offset);

		if (m_IsConsole)
		{
			m_ConsoleH = new rage::scrProgramHeaderConsole();
			m_ScriptBuff->Read((char*)m_ConsoleH, sizeof(rage::scrProgramHeaderConsole));

			/*swap endian values and that shit*/

			return EzScriptStatus::NoError;
		}

		/*PC*/
		m_PCH = new rage::scrProgramHeaderPC();
		m_ScriptBuff->Read((char*)m_PCH, sizeof(rage::scrProgramHeaderPC));

		rage::Ensure24Bits(m_PCH->m_SubHeader);
		rage::Ensure24Bits(m_PCH->m_CodeBlocksOffset);
		rage::Ensure24Bits(m_PCH->m_StaticsOffset);
		rage::Ensure24Bits(m_PCH->m_GlobalsOffset);
		rage::Ensure24Bits(m_PCH->m_NativesOffset);
		rage::Ensure24Bits(m_PCH->m_Null1);
		rage::Ensure24Bits(m_PCH->m_Null2);
		rage::Ensure24Bits(m_PCH->m_ScriptNameOffset);
		rage::Ensure24Bits(m_PCH->m_StringsOffset);

		return GetOffsetsFromHeader();
	}

	EzScriptStatus EzSrcProgram::GetOffsetsFromHeader()
	{
		if (!m_ConsoleH && !m_PCH)
			return EzScriptStatus::InvalidHeader;

		return (m_IsConsole) ? GetConsoleOffsetsFromHeader() : GetPCOffsetsFromHeader();
	}
	EzScriptStatus EzSrcProgram::GetPCOffsetsFromHeader()
	{
		m_StringBlocks = (m_PCH->m_StringsSize + 0x3FFF) >> 14;
		m_CodeBlocks = (m_PCH->m_CodeLength + 0x3FFF) >> 14;

		m_StringTableOffsets = new std::int32_t[m_StringBlocks];
		m_ScriptBuff->SetPos(m_PCH->m_StringsOffset.m_Offset + m_RSC7Offset);

		for (auto i = 0; i < m_StringBlocks; i++)
		{
			rage::PCOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::PCOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_StringTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}


		m_CodeTableOffsets = new std::int32_t[m_CodeBlocks];
		m_ScriptBuff->SetPos(m_PCH->m_CodeBlocksOffset.m_Offset + m_RSC7Offset);

		for (auto i = 0; i < m_CodeBlocks; i++)
		{
			rage::PCOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::PCOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_CodeTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}

		m_ScriptBuff->SetPos(m_PCH->m_ScriptNameOffset.m_Offset + m_RSC7Offset);
		
		char CurrentChar = 0;
		m_ScriptBuff->Read(&CurrentChar, 1);

		while (CurrentChar != 0 && CurrentChar != -1)
		{
			m_ScriptName += (char)CurrentChar;
			m_ScriptBuff->Read(&CurrentChar, 1);
		}

		auto Hash1 = rage::Joaat(m_ScriptName);
		auto Hash2 = rage::Joaat(m_ExpectedScriptName);
		auto Hash3 = m_PCH->m_NameHash;

		if (Hash1 == Hash2 && Hash1 == Hash3)
			return EzScriptStatus::NoError;

		return EzScriptStatus::NameMismatch;
	}
	EzScriptStatus EzSrcProgram::GetConsoleOffsetsFromHeader()
	{
		m_StringBlocks = (m_ConsoleH->m_StringsSize + 0x3FFF) >> 14;
		m_CodeBlocks = (m_ConsoleH->m_CodeLength + 0x3FFF) >> 14;

		m_StringTableOffsets = new std::int32_t[m_StringBlocks];
		m_ScriptBuff->SetPos(m_ConsoleH->m_StringsOffset.m_Offset + m_RSC7Offset);

		for (int i = 0; i < m_StringBlocks; i++)
		{
			rage::ConsoleOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::ConsoleOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_StringTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}


		m_CodeTableOffsets = new std::int32_t[m_CodeBlocks];
		m_ScriptBuff->SetPos(m_ConsoleH->m_CodeBlocksOffset.m_Offset + m_RSC7Offset);

		for (int i = 0; i < m_CodeBlocks; i++)
		{
			rage::ConsoleOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::ConsoleOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_CodeTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}

		m_ScriptBuff->SetPos(m_ConsoleH->m_ScriptNameOffset.m_Offset + m_RSC7Offset);

		char CurrentChar = 0;
		m_ScriptBuff->Read(&CurrentChar, 1);

		while (CurrentChar != 0 && CurrentChar != -1)
		{
			m_ScriptName += (char)CurrentChar;
			m_ScriptBuff->Read(&CurrentChar, 1);
		}

		auto Hash1 = rage::Joaat(m_ScriptName);
		auto Hash2 = rage::Joaat(m_ExpectedScriptName);
		auto Hash3 = m_ConsoleH->m_NameHash;

		if (Hash1 == Hash2 == Hash3)
			return EzScriptStatus::NoError;

		return EzScriptStatus::NameMismatch;
	}
}