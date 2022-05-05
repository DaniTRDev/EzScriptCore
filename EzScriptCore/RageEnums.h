#pragma once

namespace Ez
{
	namespace rage
	{
		enum RageInstr : std::uint8_t /*thanks to https://github.com/maybegreat48/External/blob/master/JITYSC/src/ysc.hpp*/
		{
			NOP = 0,
			IADD,
			ISUB,
			IMUL,
			IDIV,
			IMOD,
			INOT,
			INEG,
			IEQ,
			INE,
			IGT,
			IGE,
			ILT,
			ILE,
			FADD,
			FSUB,
			FMUL,
			FDIV,
			FMOD,
			FNEG,
			FEQ,
			FNE,
			FGT,
			FGE,
			FLT,
			FLE,
			VADD,
			VSUB,
			VMUL,
			VDIV,
			VNEG,
			IAND,
			IOR,
			IXOR,
			I2F,
			F2I,
			F2V,
			PUSH_CONST_U8,
			PUSH_CONST_U8_U8,
			PUSH_CONST_U8_U8_U8,
			PUSH_CONST_U32,
			PUSH_CONST_F,
			DUP,
			DROP,
			NATIVE,
			ENTER,
			LEAVE,
			LOAD,
			STORE,
			STORE_REV,
			LOAD_N,
			STORE_N,
			ARRAY_U8,
			ARRAY_U8_LOAD,
			ARRAY_U8_STORE,
			LOCAL_U8,
			LOCAL_U8_LOAD,
			LOCAL_U8_STORE,
			STATIC_U8,
			STATIC_U8_LOAD,
			STATIC_U8_STORE,
			IADD_U8,
			IMUL_U8,
			IOFFSET,
			IOFFSET_U8,
			IOFFSET_U8_LOAD,
			IOFFSET_U8_STORE,
			PUSH_CONST_S16,
			IADD_S16,
			IMUL_S16,
			IOFFSET_S16,
			IOFFSET_S16_LOAD,
			IOFFSET_S16_STORE,
			ARRAY_U16,
			ARRAY_U16_LOAD,
			ARRAY_U16_STORE,
			LOCAL_U16,
			LOCAL_U16_LOAD,
			LOCAL_U16_STORE,
			STATIC_U16,
			STATIC_U16_LOAD,
			STATIC_U16_STORE,
			GLOBAL_U16,
			GLOBAL_U16_LOAD,
			GLOBAL_U16_STORE,
			J,
			JZ,
			IEQ_JZ,
			INE_JZ,
			IGT_JZ,
			IGE_JZ,
			ILT_JZ,
			ILE_JZ,
			CALL,
			GLOBAL_U24,
			GLOBAL_U24_LOAD,
			GLOBAL_U24_STORE,
			PUSH_CONST_U24,
			SWITCH,
			STRING,
			STRINGHASH,
			TEXT_LABEL_ASSIGN_STRING,
			TEXT_LABEL_ASSIGN_INT,
			TEXT_LABEL_APPEND_STRING,
			TEXT_LABEL_APPEND_INT,
			TEXT_LABEL_COPY,
			CATCH,
			THROW,
			CALLINDIRECT,
			PUSH_CONST_M1,
			PUSH_CONST_0,
			PUSH_CONST_1,
			PUSH_CONST_2,
			PUSH_CONST_3,
			PUSH_CONST_4,
			PUSH_CONST_5,
			PUSH_CONST_6,
			PUSH_CONST_7,
			PUSH_CONST_FM1,
			PUSH_CONST_F0,
			PUSH_CONST_F1,
			PUSH_CONST_F2,
			PUSH_CONST_F3,
			PUSH_CONST_F4,
			PUSH_CONST_F5,
			PUSH_CONST_F6,
			PUSH_CONST_F7,
			BITTEST, /*added in GTA5 1.59*/
			NONE /*not an instruction*/
		};
		enum StringInstr
		{
			SI_StringFinish = 0, /*finish the current string*/
			SI_StringAddNewLine = 10,
			SI_StringAddCarriageReturn = 13,
			SI_StringAddDoubleQuote = 34,
		};

		enum RageResourceVersion
		{
			RRV_MinusThan10, /*before 1.59*/
			RRV_GreaterThan10 /*1.59 and forward, includes new opcode "bitset"*/
		};

		static std::map<RageInstr, const char*> RageInstr2Str =
		{

			{ NOP, "NOP" },
			{ IADD, "IADD" },
			{ ISUB, "ISUB" },
			{ IMUL, "IMUL" },
			{ IDIV, "IDIV" },
			{ IMOD, "IMOD" },
			{ INOT, "INOT" },
			{ INEG, "INEG" },
			{ IEQ, "IEQ" },
			{ INE, "INE" },
			{ IGT, "IGT" },
			{ IGE, "IGE" },
			{ ILT, "ILT" },
			{ ILE, "ILE" },
			{ FADD, "FADD" },
			{ FSUB, "FSUB" },
			{ FMUL, "FMUL" },
			{ FDIV, "FDIV" },
			{ FNEG, "FNEG" },
			{ FEQ, "FEQ" },
			{ FNE, "FNE" },
			{ FGT, "FGT" },
			{ FGE, "FGE" },
			{ FLT, "FLT" },
			{ FLE, "FLE" },
			{ VADD, "VADD" },
			{ VSUB, "VSUB" },
			{ VMUL, "VMUL" },
			{ VDIV, "VDIV" },
			{ VNEG, "VNEG" },
			{ IAND, "IAND" },
			{ IOR, "IOR" },
			{ IXOR, "IXOR" },
			{ I2F, "I2F" },
			{ F2I, "F2I" },
			{ F2V, "F2V" },
			{ PUSH_CONST_U8, "PUSHU8" },
			{ PUSH_CONST_U8_U8, "PUSHU16" },
			{ PUSH_CONST_U8_U8_U8, "PUSHU24" },
			{ PUSH_CONST_U32, "PUSHU32" },
			{ PUSH_CONST_F, "PUSF" },
			{ DUP, "DUP" },
			{ DROP, "DROP" },
			{ NATIVE, "NATIVE" },
			{ ENTER, "ENTER" },
			{ LEAVE, "LEAVE" },
			{ LOAD, "LOAD" },
			{ STORE, "STORE" },
			{ STORE_REV, "STORE_REV" },
			{ LOAD_N, "LOAD_N" },
			{ STORE_N, "STORE_N" },
			{ ARRAY_U8, "ARRAYU8" },
			{ ARRAY_U8_LOAD, "ARRAYU8_LOAD" },
			{ ARRAY_U8_STORE, "ARRAYU8_STORE" },
			{ LOCAL_U8, "LOCALU8" },
			{ LOCAL_U8_LOAD, "LOCALU8_LOAD" },
			{ LOCAL_U8_STORE, "LOCALU8_STORE" },
			{ STATIC_U8, "STATICU8" },
			{ STATIC_U8_LOAD, "STATICU8_LOAD" },
			{ STATIC_U8_STORE, "STATICU8_STORE" },
			{ IADD_U8, "IADDU8" },
			{ IMUL_U8, "IMULU8" },
			{ IOFFSET, "IOFFSET" },
			{ IOFFSET_U8, "IOFFSETU8" },
			{ IOFFSET_U8_LOAD, "IOFFSETU8_LOAD" },
			{ IOFFSET_U8_STORE, "IOFFSETU8_STORE" },
			{ PUSH_CONST_S16, "PUSHS16" },
			{ IADD_S16, "IADDS16" },
			{ IMUL_S16, "IMULS16" },
			{ IOFFSET_S16, "IOFFSETS16" },
			{ IOFFSET_S16_LOAD, "IOFFSET16_LOAD" },
			{ IOFFSET_S16_STORE, "IOFFSET16_STORE" },
			{ ARRAY_U16, "ARRAYU16" },
			{ ARRAY_U16_LOAD, "ARRAYU16_LOAD" },
			{ ARRAY_U16_STORE, "ARRAYU16_STORE" },
			{ LOCAL_U16, "LOCALU16" },
			{ LOCAL_U16_LOAD, "LOCALU16_LOAD" },
			{ LOCAL_U16_STORE, "LOCALU16_STORE" },
			{ STATIC_U16, "STATICU16" },
			{ STATIC_U16_LOAD, "STATICU16_LOAD" },
			{ STATIC_U16_STORE, "STATICU16_STORE" },
			{ GLOBAL_U16, "GLOBALU16" },
			{ GLOBAL_U16_LOAD, "GLOBALU16_LOAD" },
			{ GLOBAL_U16_STORE, "GLOBALU16_STORE" },
			{ J, "JMP" },
			{ JZ, "JZ" },
			{ IEQ_JZ, "IEQ_JZ" }, /*equal*/
			{ INE_JZ, "INE_JZ" }, /*not equal*/
			{ IGT_JZ, "IGT_JZ" }, /*greater than*/
			{ IGE_JZ, "IGE_JZ" }, /*greater or equal*/
			{ ILT_JZ, "ILT_JZ" }, /*lower than*/
			{ ILE_JZ, "ILE_JZ" }, /*lower or equal*/
			{ CALL, "CALL" },
			{ GLOBAL_U24, "GLOBALU24" },
			{ GLOBAL_U24_LOAD, "GLOBALU24_LOAD" },
			{ GLOBAL_U24_STORE, "GLOBALU24_STORE" },
			{ PUSH_CONST_U24, "PUSHU24" },
			{ SWITCH, "SWITCH" },
			{ STRING, "STRING" },
			{ STRINGHASH, "STRINGHASH" },
			{ TEXT_LABEL_ASSIGN_STRING, "TEXT_LABEL_ASSIGN_STRING" },
			{ TEXT_LABEL_ASSIGN_INT, "TEXT_LABEL_ASSIGN_INT" },
			{ TEXT_LABEL_APPEND_STRING, "TEXT_LABEL_APPEND_STRING" },
			{ TEXT_LABEL_APPEND_INT, "TEXT_LABEL_APPEND_INT" },
			{ TEXT_LABEL_COPY, "TEXT_LABEL_COPY" },
			{ CATCH, "CATCH" },
			{ THROW, "THROW" },
			{ CALLINDIRECT, "CALLINDIRECT" },
			{ PUSH_CONST_M1, "PUSH -1" },
			{ PUSH_CONST_0, "PUSH 0" },
			{ PUSH_CONST_1, "PUSH 1" },
			{ PUSH_CONST_2, "PUSH 2" },
			{ PUSH_CONST_3, "PUSH 3" },
			{ PUSH_CONST_4, "PUSH 4" },
			{ PUSH_CONST_5, "PUSH 5" },
			{ PUSH_CONST_6, "PUSH 6" },
			{ PUSH_CONST_7, "PUSH 7" },
			{ PUSH_CONST_FM1, "PUSH -1.f" },
			{ PUSH_CONST_F0, "PUSH 0.f" },
			{ PUSH_CONST_F1, "PUSH 1.f" },
			{ PUSH_CONST_F2, "PUSH 2.f" },
			{ PUSH_CONST_F3, "PUSH 3.f" },
			{ PUSH_CONST_F4, "PUSH 4.f" },
			{ PUSH_CONST_F5, "PUSH 5.f" },
			{ PUSH_CONST_F6, "PUSH 6.f" },
			{ PUSH_CONST_F7, "PUSH 7.f" }
		};

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
			std::uint8_t	m_SwitchOpCode;
			std::uint8_t	m_NumCases;
			FuncSwitchCase	m_Cases[1]; /*we need to do this trick to be able to have an array
										of contiguous memory*/
		};

		struct Jump
		{
			std::uint8_t  m_JumpOpCode;
			std::uint16_t m_JumpOffset;
		};

		struct NativeCall
		{
			std::uint8_t m_NativeCallOp;
			std::uint8_t m_ParamsAndReturns;
			std::uint8_t m_NativeIdHighBits;
			std::uint8_t m_NativeIdLowBits;
		};

		struct Call
		{
			std::uint8_t m_CallOp;
			std::uint8_t m_CallOff1; /*24bit value*/
			std::uint8_t m_CallOff2;
			std::uint8_t m_CallOff3;
		};
#pragma pack(pop)
	}
}