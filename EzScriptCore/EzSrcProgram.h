#pragma once

namespace Ez
{
	namespace Endians /*since we are in little endian and consoles (NOT PS4) are on big endian, we need to swap the values*/
	{ /*thanks to https://codereview.stackexchange.com/questions/151049/endianness-conversion-in-c*/
		inline std::uint16_t Reverse16(std::uint16_t value)
		{
			return (((value & 0x00FF) << 8) |
				((value & 0xFF00) >> 8));
		}

		inline std::uint32_t Reverse32(std::uint32_t value)
		{
			return (((value & 0x000000FF) << 24) |
				((value & 0x0000FF00) << 8) |
				((value & 0x00FF0000) >> 8) |
				((value & 0xFF000000) >> 24));
		}
	}

	namespace rage
	{
		/// <summary>
		///		<param name='Str'>The string that is going to be joaated</param> 
		/// </summary> It hashes the given string using Jenkins-One-At-A-Time
		/// <returns>std::uint32_t</returns>
		extern std::uint32_t Joaat(std::string Str);

		typedef struct
		{
			std::int32_t m_Offset;
			char m_Pad[4];
		} PCOffsetEntry;

		typedef struct
		{
			std::int32_t m_Offset; /*this will be in little endian, needs conversion*/
		}ConsoleOffsetEntry;

		/// <summary>
		///		<param name='Entry'>The offset entry that is going to be forced to be 24bits</param> 
		/// </summary> It does: Entry.m_Offset = Entry.m_Offset & 0xFFFFFF;
		extern void Ensure24Bits(PCOffsetEntry& Entry);

		/// <summary>
		///		<param name='Entry'>The offset entry that is going to be forced to be 24bits</param> 
		/// It does: Entry.m_Offset = Reverse32(Entry.m_Offset) & 0xFFFFFF;
		/// Since this is for old gen consoles it also swaps endians
		/// </summary> 
		extern void Ensure24Bits(ConsoleOffsetEntry& Entry);

		struct scrProgramHeaderPC
		{
			std::int32_t			m_Magic;
			char m_Pad1[4];
			PCOffsetEntry			m_SubHeader;
			PCOffsetEntry			m_CodeBlocksOffset;
			std::int32_t			m_GlobalsVersion;
			std::int32_t			m_CodeLength;
			std::int32_t			m_ParameterCount;
			std::int32_t			m_StaticsCount;
			std::int32_t			m_GlobalsCount;
			std::int32_t			m_NativesCount;
			PCOffsetEntry			m_StaticsOffset;
			PCOffsetEntry			m_GlobalsOffset;
			PCOffsetEntry			m_NativesOffset;
			PCOffsetEntry			m_Null1;
			PCOffsetEntry			m_Null2;
			std::int32_t			m_NameHash;
			std::int32_t			m_Null3;
			PCOffsetEntry			m_ScriptNameOffset;
			PCOffsetEntry			m_StringsOffset;
			std::int32_t			m_StringsSize;
			char					m_Pad2[4];
			std::int32_t			m_Null4;
			char					m_Pad3[4];
		};
		struct scrProgramHeaderConsole /*THESE VALUES NEED TO BE TRANSFORMED FROM BIG ENDIA TO LITTLE ENDIAN USING rage::Reverse32*/
		{
			std::int32_t				m_Magic;
			ConsoleOffsetEntry			m_SubHeader;
			ConsoleOffsetEntry			m_CodeBlocksOffset;
			std::int32_t				m_GlobalsVersion;
			std::int32_t				m_CodeLength;
			std::int32_t				m_ParameterCount;
			std::int32_t				m_StaticsCount;
			std::int32_t				m_GlobalsCount;
			std::int32_t				m_NativesCount;
			ConsoleOffsetEntry			m_StaticsOffset;
			ConsoleOffsetEntry			m_GlobalsOffset;
			ConsoleOffsetEntry			m_NativesOffset;
			ConsoleOffsetEntry			m_Null1;
			ConsoleOffsetEntry			m_Null2;
			std::int32_t				m_NameHash;
			std::int32_t				m_Null3;
			ConsoleOffsetEntry			m_ScriptNameOffset;
			ConsoleOffsetEntry			m_StringsOffset;
			std::int32_t				m_StringsSize;
			std::int32_t				m_Null4;
		};
	}

	enum class EzScriptStatus
	{
		NoError,
		AlreadyParsed,
		InvalidHeader,
		NameMismatch,
	};

	class EzDecompiler;

	struct EzSrcProgram
	{
		friend class EzDecompiler;

		/// <summary>
		/// </summary> Sets variables to 0
		EzSrcProgram();

		/// <summary>
		///		<param name='Stream'>The stream of the script file, should be derived from a std::ifstream</param> 
		///		<param name='ExpectedSriptName'>The original script name, used to check if the parsing was good</param> 
		///		<param name='IsConsole'>Tells our parser to parse as a console header</param>  
		/// </summary> 
		/// <returns>EzScriptStatus</returns>
		EzScriptStatus ParseHeader(std::istream& Stream, std::string ExpectedScriptName, bool IsConsole); 
		/// <summary>
		/// </summary> Retrieves the offsets for block, strings and their array size
		/// <returns>EzScriptStatus</returns>
		EzScriptStatus GetOffsetsFromHeader(); /*retrieve the variables above from the script header*/

		rage::scrProgramHeaderConsole*	m_ConsoleH;
		rage::scrProgramHeaderPC*		m_PCH;

		bool							m_IsConsole;

		std::int32_t					m_RSC7Offset;
		std::int32_t*					m_StringTableOffsets;
		std::int32_t*					m_CodeTableOffsets;
		std::int32_t					m_StringBlocks;
		std::int32_t					m_CodeBlocks;
		std::string						m_ExpectedScriptName; /*the expected script name, will be compared against m_ScriptName*/
		std::string						m_ScriptName; /*retrieved throught the header information*/
		bool							m_RSC7;

	private:

		EzScriptStatus GetPCOffsetsFromHeader();
		EzScriptStatus GetConsoleOffsetsFromHeader();


		std::unique_ptr<EzBuffer> m_ScriptBuff;
	};

}
