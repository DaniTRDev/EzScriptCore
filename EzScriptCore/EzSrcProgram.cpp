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
	}


	EzSrcProgram::EzSrcProgram() : m_Header(nullptr), m_RSC7Offset(0), m_StringTableOffsets(nullptr),
		m_CodeTableOffsets(nullptr), m_StringBlocks(0), m_CodeBlocks(0), m_ExpectedScriptName(""), m_ScriptName(""), 
		m_RSC7(0), m_ScriptBuff(std::make_unique<EzBuffer>())
	{
	}

	EzScriptStatus EzSrcProgram::ParseHeader(std::istream& Stream, std::string ExpectedScriptName)
	{
		if (m_Header)
			return EzScriptStatus::AlreadyParsed;

		if (auto Result = m_ScriptBuff->FromStream(Stream); Result != EzBufferStatus::NoError)
			throw std::runtime_error("Error while parsing script header!");

		std::int32_t Magic = 0;
		m_ScriptBuff->Read((char*)&Magic, sizeof(Magic));

		if (Magic == 0x37435352)
			m_RSC7Offset = 0x10;

		m_RSC7 = (m_RSC7Offset != 0);
		m_ExpectedScriptName = ExpectedScriptName;

		m_ScriptBuff->SetPos(m_RSC7Offset);

		m_Header = new rage::scrProgramHeaderPC();
		m_ScriptBuff->Read((char*)m_Header, sizeof(rage::scrProgramHeaderPC));

		rage::Ensure24Bits(m_Header->m_SubHeader);
		rage::Ensure24Bits(m_Header->m_CodeBlocksOffset);
		rage::Ensure24Bits(m_Header->m_StaticsOffset);
		rage::Ensure24Bits(m_Header->m_GlobalsOffset);
		rage::Ensure24Bits(m_Header->m_NativesOffset);
		rage::Ensure24Bits(m_Header->m_Null1);
		rage::Ensure24Bits(m_Header->m_Null2);
		rage::Ensure24Bits(m_Header->m_ScriptNameOffset);
		rage::Ensure24Bits(m_Header->m_StringsOffset);

		return GetOffsetsFromHeader();
	}
	EzScriptStatus EzSrcProgram::GetOffsetsFromHeader()
	{
		if (!m_Header)
			return EzScriptStatus::InvalidHeader;

		m_StringBlocks = (m_Header->m_StringsSize + 0x3FFF) >> 14;
		m_CodeBlocks = (m_Header->m_CodeLength + 0x3FFF) >> 14;

		m_StringTableOffsets = new std::int32_t[m_StringBlocks];
		m_ScriptBuff->SetPos(m_Header->m_StringsOffset.m_Offset + m_RSC7Offset);

		for (auto i = 0; i < m_StringBlocks; i++)
		{
			rage::PCOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::PCOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_StringTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}

		m_CodeTableOffsets = new std::int32_t[m_CodeBlocks];
		m_ScriptBuff->SetPos(m_Header->m_CodeBlocksOffset.m_Offset + m_RSC7Offset);

		for (auto i = 0; i < m_CodeBlocks; i++)
		{
			rage::PCOffsetEntry Entry;
			m_ScriptBuff->Read((char*)&Entry, sizeof(rage::PCOffsetEntry));

			rage::Ensure24Bits(Entry);
			m_CodeTableOffsets[i] = Entry.m_Offset + m_RSC7Offset;
		}

		m_ScriptBuff->SetPos(m_Header->m_ScriptNameOffset.m_Offset + m_RSC7Offset);

		char CurrentChar = 0;
		m_ScriptBuff->Read(&CurrentChar, 1);

		while (CurrentChar != 0 && CurrentChar != -1)
		{
			m_ScriptName += (char)CurrentChar;
			m_ScriptBuff->Read(&CurrentChar, 1);
		}

		auto Hash1 = rage::Joaat(m_ScriptName);
		auto Hash2 = (m_ExpectedScriptName.size() != 0) ? rage::Joaat(m_ExpectedScriptName) : Hash1;
		auto Hash3 = m_Header->m_NameHash;

		if (Hash1 == Hash2 && Hash1 == Hash3)
			return EzScriptStatus::NoError;

		return EzScriptStatus::NameMismatch;
	}
}