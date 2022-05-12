#pragma once

namespace Ez
{
	enum class EzDisassemblerStatus;

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
			std::uintptr_t EndAddress, std::uintptr_t Index, rage::FuncPrologue * Prologue, rage::FuncEpilogue* Epilogue);
		~EzFunction();

		void MapInstructions();
		std::size_t GetMappedInstructions();
		const std::vector<std::shared_ptr<EzInstruction>>& GetInstructions();

		void AddLoc(std::uintptr_t Address);
		const std::map<std::uintptr_t, std::uintptr_t>& GetLocs();
		std::uintptr_t GetLocIdByOffset(std::uintptr_t Offset);

		bool IsAddressALoc(std::uintptr_t Address);

		std::size_t GetFuncIndex();

		std::uintptr_t GetFuncStartAddr();
		std::uintptr_t GetFuncEndAddr();
		
		rage::FuncPrologue* GetPrologue();
		rage::FuncEpilogue* GetEpilogue();

	private:

		void GetJump(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uintptr_t FuncEnd);
		void GetDup(rage::RageInstr Instr, std::uintptr_t& OpCodeId);

		void GetSwitch(rage::RageInstr Instr, std::uintptr_t& OpCodeId);

		void GetCall(rage::RageInstr Instr, std::uintptr_t& OpCodeId);
		void GetNativeCall(rage::RageInstr Instr, std::uintptr_t& OpCodeId);

		void GetVarInstr(rage::RageInstr Instr, std::uintptr_t& OpCodeId, 
			std::uint8_t Size, bool Signed); /*globals, locals and statics*/

		void GetFloatPush(rage::RageInstr Instr, std::uintptr_t& OpCodeId);

		void GetSigned(rage::RageInstr Instr, std::uintptr_t& OpCodeId, std::uint8_t Size);

	private:

		/*an unique_ptr so when we clear this vector the memory will be freed automatically*/
		std::vector<std::shared_ptr<EzInstruction>> m_Instructions;
		std::size_t m_MappedInstructions;

		/*address, ID*/
		std::map<std::uintptr_t, std::uintptr_t> m_Locs;

		std::uint8_t* m_OpCodes; 

	private:

		std::uintptr_t m_StartAddr;
		std::uintptr_t m_EndAddr;

		std::uintptr_t m_Index;

		rage::FuncPrologue* m_Prologue;
		rage::FuncEpilogue* m_Epilogue;
	};
}

