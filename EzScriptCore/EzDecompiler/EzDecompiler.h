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
		///		<param name=''>An EzSrcProgram that will contain the require information to decompile the script</param>
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
		/// </summary> Returns script parameters
		/// <returns>std::vector<rage::EzScriptParam></returns>
		const std::vector<rage::EzScriptParam>& GetScriptParams();
		/// <summary>
		/// </summary> Returns mapped strings
		/// <returns>std::vector<std::string></returns>
		const std::vector<std::string>& GetStrings();
		/// <summary>
		/// </summary> Returns the number of blocks
		/// <returns>std::size_t</returns>
		const std::size_t GetBlocksNumber();
		/// <summary>
		/// </summary> Returns a block size after mergin all the blocks in an only one
		/// <returns>std::vector<rage::EzLocal></returns>
		const std::size_t GetBlockSize();

		const std::vector<EzFunction*>& GetFunctions();
		/// <summary>
		/// </summary> Returns the number of predecompiled or decompiled instructions
		/// <returns>std::size_t</returns>
		std::size_t GetDecompiledInstructions();

	public: /*decompilation*/

		/// <summary>
		/// </summary>  Map locals indexes of the script
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapLocals();

		/// <summary>
		/// </summary>  Map strings into an array
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapStrings();

		/// <summary>
		/// </summary>  Map code blocks and merge them in a single block
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapCodeBlocks();

		/// <summary>
		/// </summary>  Map functions from the block of MapCodeBlocks
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus MapFunctions();

		/// <summary>
		/// </summary>Decompiles functions mapped in MapFunctions
		/// <returns>EzDecompilerStatus</returns>
		EzDecompilerStatus PreDecompileFunctions();

		EzDecompilerStatus Decompile();

		std::ostringstream& GetAssembly();

	private:

		EzDecompilerStatus MapLocals(std::int32_t StaticsOffset, std::int32_t StaticsCount, std::int32_t ParameterCount);

		EzDecompilerStatus MapStrings(std::int32_t StringsSize);

		EzDecompilerStatus MapCodeBlocks(std::int32_t CodeLenght);

		EzDecompilerStatus MapFunctionProto(std::uintptr_t FuncStartAddr, std::uintptr_t FuncEndAddr);

	private: /*decompiled things*/

		std::vector<rage::EzLocal> m_Locals;
		std::vector<rage::EzScriptParam> m_ScriptParams;
		std::vector<std::string> m_Strings;

		std::unique_ptr<EzBuffer> m_CodeBlock;
		std::vector<EzInstruction*> m_Instructions;
		std::size_t m_DecompiledInstructions;

		std::vector<EzFunction*> m_Functions;

		std::ostringstream m_Assembly;

	private: /*variables require by the decompiler*/

		std::unique_ptr<EzBuffer> m_ScriptBuffer;

		std::uint8_t m_OpCodeId; /*used to get the last addr we scanned*/
		EzFunction* m_LastFunc; /*used to get th last func we wanted to decompile*/

		EzSrcProgram* m_Script;
	};
}

