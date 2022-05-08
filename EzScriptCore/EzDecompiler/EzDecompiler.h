#pragma once
#include "EzVariables.h"

namespace Ez
{
	enum class EzDecompilerStatus
	{
		InvalidScriptHeader,
		CodeBlocksNotMapped,
		LocalsNotMapped,
		StaticParamsNotMapped,
		GlobalsNotMapped,
		FunctionsNotMapped, /*Mapped != decompiled*/
		InstructionsNotDecompiled,
		InvalidFuncEntersLeave, /*the number of enters and leaves is not equal*/
		UnexpectedOpCode,
		InvalidVariableCount,
		DuplicatedSwitchCase,
		NoError
	};

	class EzDecompiler 
	{
	public:

		/// <summary>
		///		<param name='Script'>An EzSrcProgram that will contain the require information to decompile the script</param>
		/// </summary> 
		/// <returns>EzDecompilerStatus</returns>
		EzDecompiler(EzSrcProgram* Script); /*will need to transfer the ownership of the m_Buffer to avoid accesing it when
									   decompiling*/
		~EzDecompiler();

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
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapScript();

		/// <summary>
		/// </summary>  Map locals indexes of the script
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapLocals();

		/// <summary>
		/// </summary>  Map strings into an array
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapStrings();

		/// <summary>
		/// </summary>  Map natives and their linked index in this script
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapNatives();

		/// <summary>
		/// </summary>  Map code blocks and merge them in a single block
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapCodeBlocks();

		/// <summary>
		/// </summary>  Map functions from the block of MapCodeBlocks
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapFunctions();

		/// <summary>
		/// <param name='Offset'>The offset where the function is located, or any of its instructions</param>
		/// </summary> Retrieves the function from the given offset
		/// <returns>EzFunction*</returns>
		EzFunction* GetFuncFromOffset(std::uintptr_t Offset);

		/// <summary>
		/// </summary>Maps and parse the instructions inside every mapped function
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapInstructionsFromFuncs();

		/// <summary>
		/// </summary> Returns the number of mapped instructions
		/// <returns>std::size_t</returns>
		std::size_t GetMappedInstructions();

		/// <summary>
		/// </summary> Disassembles every function inside m_Functions
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus Disassemble();

		/// <summary>
		/// <param name='Func'>The function to be disassembled</param>
		/// </summary> Disassembles the function passed as argument
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus DisassembleFunc(EzFunction* Func);

		/// <summary>
		/// <param name='Func'>The function in where the instruction is located</param>
		/// <param name='Instructions'>The array of instructions of this function </param>
		/// <param name='InstrId'>A reference to the instruction id variable</param>
		/// </summary> Disassembles the instruction passed on the array with the current Index
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus DisassembleInstr(EzFunction * Func, std::shared_ptr<EzInstruction>* Instructions, std::uintptr_t& InstrId);

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

		EzDecompilerStatus MapFunctionProto(std::uintptr_t FuncStartAddr, std::uintptr_t FuncEndAddr);

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

	private: /*variables require by the decompiler*/

		std::unique_ptr<EzBuffer> m_ScriptBuffer;
		std::unique_ptr<EzBuffer> m_CodeBuffer;
		EzSrcProgram* m_Script;

		std::uint8_t m_OpCodeId; /*used to get the last addr we scanned*/
	};
}

