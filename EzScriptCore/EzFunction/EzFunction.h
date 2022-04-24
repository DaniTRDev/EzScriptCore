#pragma once

namespace Ez
{
	namespace rage
	{
#pragma pack(push, 1) /*disable automatic padding of VS to be "cache-friendly"*/
		struct FuncPrologue
		{
			std::uint8_t	m_EnterOpCode;
			std::uint8_t	m_ParemeterCount;
			std::uint16_t	m_VariableCount; /*need to swap endians if it's from console*/
			std::uint8_t	m_FuncNameLenght;
		};
		struct FuncEpilogue
		{
			std::uint8_t m_LeaveOpCode;
			std::uint8_t m_ParameterCount;
			std::uint8_t m_ReturnCount;
		};

		struct FuncSwitchCase
		{
			std::int32_t m_CaseVal;
			std::int16_t m_JumpOffset;
		};
		struct FuncSwitch
		{
			std::uint8_t	m_OpSwitchOpCode;
			std::uint8_t	m_NumCases;
			FuncSwitchCase	m_Cases[1]; /*we need to do this trick to be able to have an array 
									    of contiguous memory*/
		};

		struct Jump
		{
			std::uint8_t  m_JumpOpCode;
			std::uint16_t m_JumpOffset;
		};
#pragma pack(pop)
	};

	enum class EzDecompilerStatus;

	class EzFunction
	{
	public:

		/// <summary>
		/// 	<param name='OpCodes'>An array of RAGE VM byte codes that will be used to get the instructions of this func</param>
		///		<param name='StartAddress'>The start address of the function</param>
		///		<param name='EndAddress'>The end address of the function</param> 
		///		<param name='Prologue'>A ptr to a struct that contains the prologue information</param> 
		///		<param name='Epilogue'>A ptr to a struct that contains the epilogue information</param> 
		/// </summary>  Retrieves the numer of arguments 
		EzFunction(std::uint8_t* OpCodes, std::uintptr_t StartAddres,
			std::uintptr_t EndAddress, rage::FuncPrologue * Prologue, rage::FuncEpilogue* Epilogue);
		~EzFunction();

		EzDecompilerStatus PreDecompile(bool Console);
		EzDecompilerStatus Decompile(bool Console);

		virtual std::size_t GetDecompiledInstrNum();

		std::ostringstream& GetAssembly();

	private:

		EzDecompilerStatus GetJump(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uintptr_t FuncEnd, bool Console);
		EzDecompilerStatus GetDup(rage::RageInstr Instr, std::uintptr_t& OpCodeId);

		EzDecompilerStatus GetSwitch(rage::RageInstr Instr, std::uintptr_t& OpCodeId, bool Console);

	private:

		/*an unique_ptr so when we clear this vector the memory will be freed automatically*/
		std::vector<std::unique_ptr<EzInstruction>> m_Instructions; 
		std::uint8_t* m_OpCodes; 

		EzStack m_Stack;

		std::size_t m_DecompiledInstructions;

		std::ostringstream m_Dissasembly;

	private:

		std::uintptr_t m_StartAddr;
		std::uintptr_t m_EndAddr;

		rage::FuncPrologue* m_Prologue;
		rage::FuncEpilogue* m_Epilogue;
	};
}

