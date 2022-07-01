#pragma once

namespace Ez
{
	class EzAsmInstruction
	{
	public:

		friend class EzAssemblerStateNode;

		EzAsmInstruction(rage::RageInstr Instr, std::size_t OperandCount);
		EzAsmInstruction(rage::RageInstr Instr);
		~EzAsmInstruction();

		rage::RageInstr GetInstructionId();

		void SaveOperand(char* Source);
		std::size_t GetOperandCount();

		std::int32_t					GetOperandsI32();
		std::uint32_t					GetOperandsU32();

		float							GetOperandsF();

		std::uint16_t					GetNativeIndex(); /*native*/
		std::uint8_t					GetNativeParamCount();
		std::uint8_t					GetNativeReturnCount();

		std::uintptr_t GetJumpOffset();
		std::string_view GetFuncCall(); /*if this instr == CALL this will contain the name of the function to call*/

		const std::map<std::int32_t, std::uintptr_t>& GetSwitchCases();

		bool IsSigned();
		bool IsFloat();

		bool IsJump();
		bool IsCall();
		bool IsNativeCall();
		bool IsVarPush();
		bool IsVarLoad();
		bool IsVarStore();
		bool IsStatic();
		bool IsLocal();
		bool IsGlobal();
		bool IsSwitch();

	private:

		rage::RageInstr					m_InstructionId;

		bool							m_IsJump;
		bool							m_IsCall;
		bool							m_IsNativeCall;
		bool							m_IsGlobal; /*script globals*/
		bool							m_IsLocal; /*func params / variables*/
		bool							m_IsStatic; /*script locals / params*/
		bool							m_IsVarPush; /*linked to these 3 bools above*/
		bool							m_IsVarLoad; /**/
		bool							m_IsVarStore; /**/
		bool							m_IsSwitch; /*is this instruction from a switch*/
		bool							m_IsOperandSigned; /*distinguis between intX and uintX*/
		bool							m_IsFloat; /*is operand a float*/

		std::uint16_t					m_NativeIndex; /*NATIVES START*/
		std::uint8_t					m_NativeParameterCount;
		std::uint8_t					m_NativeReturnCount; /*NATIVE END*/

		std::map<std::int32_t, std::uintptr_t> m_Cases; /*SWITCH START SWITCH END*/

		std::uintptr_t m_JumpOffset; 
		std::string_view m_FuncCall;

		char* m_Operands;
		std::size_t m_OperandCount;
	};
}

