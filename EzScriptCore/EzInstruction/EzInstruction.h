#pragma once

namespace Ez
{
	class EzFunction;

	enum class EzOperandValue
	{
		Int, /*any size, this will be determined automatically by the operand number*/
		Uint,
		Float
	};

	class EzInstruction
	{
	public:

		friend class EzFunction;
		friend class EzDecompiler;

		EzInstruction(rage::RageInstr Instr, std::uintptr_t StartAddr, std::uint8_t OperandCount);
		~EzInstruction();

		std::uintptr_t GetStartAddr();
		std::uintptr_t GetEndAddr();

		rage::RageInstr GetInstructionId();
		std::string_view Id2String();

		std::size_t GetInstructionSize();

		std::int32_t					GetOperandsI32(std::uint8_t* OpCodes);
		std::uint32_t					GetOperandsU32(std::uint8_t* OpCodes);

		float							GetOperandsF(std::uint8_t* OpCodes);

		std::uint16_t					GetNativeIndex(); /*native*/
		std::uint8_t					GetNativeParamCount();
		std::uint8_t					GetNativeReturnCount();

		std::uintptr_t GetJumpOffset();
		std::uintptr_t GetCallOffset();
		
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


		std::uint8_t GetOperandCount();

	protected:

		rage::RageInstr					m_InstructionId;

		bool							m_IsJump;
		bool							m_IsCall;
		bool							m_IsNativeCall;
		bool							m_IsGlobal; /*script globals*/
		bool							m_IsLocal;/*func params / variables*/
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

		std::uintptr_t m_JumpOffset; /*JUMP START -- RELATIVE TO OPCODES BUFFER -- JUMP END*/
		std::uintptr_t m_CallOffset;/*CALL START -- RELATIVE TO OPCODES BUFFER -- CALL END*/

		std::uintptr_t					m_StartAddr;
		std::uintptr_t					m_EndAddr;

		std::size_t m_InstructionSize;

		std::uint8_t					m_OperandCount;

		std::ostringstream				m_DecompiledInstruction;

		char* m_Operands;
	};
}
