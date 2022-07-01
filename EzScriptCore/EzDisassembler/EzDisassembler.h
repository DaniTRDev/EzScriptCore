#pragma once
#include "EzVariables.h"

namespace Ez
{

	class EzDisassembler 
	{
	public:

		/// <summary>
		///		<param name='Script'>An EzSrcProgram that will contain the require information to decompile the script</param>
		/// </summary> 
		
		EzDisassembler(EzSrcProgram* Script); /*will need to transfer the ownership of the m_Buffer to avoid accesing it when
									   decompiling*/
		~EzDisassembler();

		/// <summary>
		/// </summary> Returns mapped locals
		/// <returns>std::vector<rage::EzLocal></returns>
		const std::vector<rage::EzLocal>& GetLocals();

		/// <summary>
		/// </summary> Returns mapped script parameters
		/// <returns>std::vector<rage::EzScriptParam></returns>
		const std::vector<rage::EzScriptParam>& GetScriptParams();

		/// <summary>
		/// </summary> Returns mapped strings
		/// <returns>std::vector<std::string></returns>
		const std::vector<std::string>& GetStrings();

		/// <summary>
		/// </summary> Returns mapped natives and their indices
		/// <returns>std::map<std::size_t, std::uint64_t></returns>
		const std::map<std::size_t, std::uint64_t>& GetNatives();

		/// <summary>
		/// </summary> Returns the number of blocks
		/// <returns>std::size_t</returns>
		const std::size_t GetBlocksNumber();

		/// <summary>
		/// </summary> Returns a block size after mergin all the blocks in an only one
		/// <returns>std::size_t</returns>
		const std::size_t GetCodeSize();

		/// <summary>
		/// </summary> Returns a block size after mergin all the blocks in an only one
		/// <returns>std::vector<rage::EzLocal></returns>
		std::uint8_t* GetCode();

		/// <summary>
		/// </summary> Returns mapped functions
		/// <returns>std::vector<rage::EzFunction*></returns>
		const std::vector<EzFunction*>& GetFunctions();

		/// <summary>
		/// </summary> Returns the name of the script
		/// <returns>std::string</returns>
		std::string GetScriptName();

	public: /*decompilation*/

		/// <summary>
		/// </summary>  Map everything from the script	
		void MapScript();

		/// <summary>
		/// </summary>  Map locals indexes of the script
		void MapLocals();

		/// <summary>
		/// </summary>  Map strings into an array
		void MapStrings();

		/// <summary>
		/// </summary>  Map natives and their linked index in this script
		void MapNatives();

		/// <summary>
		/// </summary>  Map code blocks and merge them in a single block
		void MapCodeBlocks();

		/// <summary>
		/// </summary>  Map functions from the block of MapCodeBlocks
		void MapFunctions();

		/// <summary>
		/// <param name='Offset'>The offset where the function is located, or any of its instructions</param>
		/// </summary> Retrieves the function from the given offset
		/// <returns>EzFunction*</returns>
		EzFunction* GetFuncFromOffset(std::uintptr_t Offset);

		/// <summary>
		/// </summary>Maps and parse the instructions inside every mapped function
		void MapInstructionsFromFuncs();

		/// <summary>
		/// </summary> Returns the number of mapped instructions
		/// <returns>std::size_t</returns>
		std::size_t GetMappedInstructions();

		/// <summary>
		/// </summary> Disassembles every function inside m_Functions
		void Disassemble();

		/// <summary>
		/// <param name='Func'>The function to be disassembled</param>
		/// </summary> Disassembles the function passed as argument
		void DisassembleFunc(EzFunction* Func);

		/// <summary>
		/// <param name='Func'>The function in where the instruction is located</param>
		/// <param name='Instructions'>The array of instructions of this function </param>
		/// <param name='InstrId'>A reference to the instruction id variable</param>
		/// </summary> Disassembles the instruction passed on the array with the current Index
		void DisassembleInstr(EzFunction * Func, EzPtr<EzInstruction>* Instructions, std::uintptr_t& InstrId);

		/// <summary>
		/// </summary> Returns the number of disassembled instructions
		/// <returns>std::size_t</returns>
		std::size_t GetDisassembledInstructions();

		/// <summary>
		/// <param name='Address'>The address where the instruction is</param>
		/// </summary> Returns the id(in the vector of instructions) of the given addr
		/// <returns>std::size_t</returns>
		std::size_t GetInstructionPosFromAddr(std::uintptr_t Address);

		/// <summary>
		/// </summary> Returns the disassembled code to text
		/// <returns>std::ostringstream&</returns>
		std::ostringstream& GetAssembly();

		/// <summary>
		/// </summary> Returns the last disassembled code to text
		/// <returns>std::ostringstream&</returns>
		std::ostringstream& GetLastAssembly();

		/// <summary>
		/// </summary> Frees m_Assembly & m_LastAssembly
		void FreeDisassemblyBuffer();

	private:

		void MapFunctionProto(std::uintptr_t FuncStartAddr, std::uintptr_t FuncEndAddr);
		
		void RetrieveMapsFromFile(); /*fills out locals, params, ... with data from file*/
		void SaveMappedDataToFile(); /*save mapped data to the script .dat file*/

	private: /*decompiled things*/

		std::vector<rage::EzLocal> m_Locals;
		std::vector<rage::EzScriptParam> m_ScriptParams;
		std::vector<std::string> m_Strings;
		std::map<std::size_t, std::uint64_t> m_NativeMap;

		std::size_t m_MappedInstructions;
		std::size_t m_DisassembledInstructions;

		std::vector<EzFunction*> m_Functions;

		std::ostringstream m_Assembly;
		std::ostringstream m_LastAssembly; /*the disassembled code of the func from DisassembleFunc*/
		
		std::fstream m_ScriptDatFile; /*a file that will contain the mapped strings, locals, params,
							  native maps and so on so we can save time when re-opening a script*/

	private: /*variables require by the decompiler*/

		std::unique_ptr<EzBuffer> m_ScriptBuffer;
		std::unique_ptr<EzBuffer> m_CodeBuffer;
		EzSrcProgram* m_Script;

		std::uint8_t m_OpCodeId; /*used to get the last addr we scanned*/
	};
}

